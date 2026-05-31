#include "uados/planning/strategic_planner.hpp"
#include "uados/planning/behavior_planner.hpp"
#include "uados/planning/motion_planner.hpp"
#include "uados/localization/hdmap_engine.hpp"

#include <gtest/gtest.h>
#include <chrono>

using namespace uados;
using namespace uados::planning;
using namespace uados::localization;

class PlanningSubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize Components
        uados::core::Config mock_config;
        
        ASSERT_EQ(strategic_planner_.init(mock_config), Status::Ok);
        ASSERT_EQ(behavior_planner_.init(mock_config), Status::Ok);
        ASSERT_EQ(motion_planner_.init(mock_config), Status::Ok);

        ASSERT_EQ(strategic_planner_.start(), Status::Ok);
        ASSERT_EQ(behavior_planner_.start(), Status::Ok);
        ASSERT_EQ(motion_planner_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(strategic_planner_.stop(), Status::Ok);
        EXPECT_EQ(behavior_planner_.stop(), Status::Ok);
        EXPECT_EQ(motion_planner_.stop(), Status::Ok);
    }

    StrategicPlanner strategic_planner_;
    BehaviorPlanner behavior_planner_;
    MotionPlanner motion_planner_;
};

// ============================================================================
// 1. Strategic Routing Planner Tests
// ============================================================================

TEST_F(PlanningSubsystemTest, TestStrategicRouteGenerationSuccess) {
    // Normal routing path across the mock layout
    auto route = strategic_planner_.compute_route("lanelet_1001", "lanelet_1004");
    ASSERT_EQ(route.size(), 4);
    EXPECT_EQ(route[0], "lanelet_1001");
    EXPECT_EQ(route[1], "lanelet_1002");
    EXPECT_EQ(route[2], "lanelet_1003");
    EXPECT_EQ(route[3], "lanelet_1004");
}

TEST_F(PlanningSubsystemTest, TestStrategicRouteSingleHops) {
    // Route starting and ending at same segment
    auto route = strategic_planner_.compute_route("lanelet_1002", "lanelet_1002");
    ASSERT_EQ(route.size(), 1);
    EXPECT_EQ(route[0], "lanelet_1002");
}

TEST_F(PlanningSubsystemTest, TestStrategicRouteInvalidPath) {
    // Attempt routing from terminal node backwards
    auto route = strategic_planner_.compute_route("lanelet_1004", "lanelet_1001");
    EXPECT_TRUE(route.empty());

    // Attempt routing unconnected or missing node
    auto route_invalid = strategic_planner_.compute_route("lanelet_9999", "lanelet_1004");
    EXPECT_TRUE(route_invalid.empty());
}

// ============================================================================
// 2. Behavior Decision Planner Tests
// ============================================================================

TEST_F(PlanningSubsystemTest, TestBehaviorCruiseMode) {
    Pose ego_pose;
    ego_pose.position = {10.0, 0.0, 0.0};
    Velocity3D ego_vel{10.0, 0.0, 0.0};

    LaneletInfo current_lanelet;
    current_lanelet.id = "lanelet_1001";
    current_lanelet.speed_limit_mps = 13.8;
    current_lanelet.is_intersection = false;
    current_lanelet.has_stop_line = false;

    std::vector<std::string> route = {"lanelet_1001", "lanelet_1002"};
    std::vector<DetectedObject> obstacles;

    // Plan under Green Light -> Cruising at limit
    auto decision = behavior_planner_.plan(
        ego_pose, ego_vel, route, current_lanelet, obstacles, TrafficLightState::Green);
    
    EXPECT_EQ(decision.state, BehaviorState::Cruise);
    EXPECT_NEAR(decision.target_speed, 13.8, 0.01);
    EXPECT_FALSE(decision.emergency_braking);
}

TEST_F(PlanningSubsystemTest, TestBehaviorStopAtStopLineTrigger) {
    Pose ego_pose;
    ego_pose.position = {50.0, 0.0, 0.0}; // Near stop line (which is at 58m, dist = 8m)
    Velocity3D ego_vel{8.0, 0.0, 0.0};

    LaneletInfo current_lanelet;
    current_lanelet.id = "lanelet_1002";
    current_lanelet.speed_limit_mps = 8.33;
    current_lanelet.is_intersection = false;
    current_lanelet.has_stop_line = true;
    current_lanelet.stop_line_distance = 8.0;

    std::vector<std::string> route = {"lanelet_1002", "lanelet_1003"};
    std::vector<DetectedObject> obstacles;

    // Plan under RED light -> Transition to stop mode
    auto decision = behavior_planner_.plan(
        ego_pose, ego_vel, route, current_lanelet, obstacles, TrafficLightState::Red);
    
    EXPECT_EQ(decision.state, BehaviorState::StopAtStopLine);
    EXPECT_NEAR(decision.target_speed, 0.0, 0.01);
    EXPECT_NEAR(decision.stop_distance, 8.0, 0.1);
    EXPECT_FALSE(decision.emergency_braking);
}

TEST_F(PlanningSubsystemTest, TestBehaviorYieldObstacleTrigger) {
    Pose ego_pose;
    ego_pose.position = {20.0, 0.0, 0.0};
    Velocity3D ego_vel{10.0, 0.0, 0.0};

    LaneletInfo current_lanelet;
    current_lanelet.id = "lanelet_1001";
    current_lanelet.speed_limit_mps = 13.8;

    std::vector<std::string> route = {"lanelet_1001"};
    
    // Static obstacle 8 meters directly ahead
    DetectedObject obs;
    obs.id = 42;
    obs.position = {28.0, 0.0, 0.0};
    obs.velocity = {0.0, 0.0, 0.0};

    std::vector<DetectedObject> obstacles = {obs};

    auto decision = behavior_planner_.plan(
        ego_pose, ego_vel, route, current_lanelet, obstacles, TrafficLightState::Green);
    
    EXPECT_EQ(decision.state, BehaviorState::YieldObstacle);
    EXPECT_LT(decision.target_speed, 5.0); // Smooth deceleration
    EXPECT_NEAR(decision.stop_distance, 8.0, 0.1);
    EXPECT_FALSE(decision.emergency_braking);
}

TEST_F(PlanningSubsystemTest, TestBehaviorEmergencyStopTrigger) {
    Pose ego_pose;
    ego_pose.position = {20.0, 0.0, 0.0};
    Velocity3D ego_vel{10.0, 0.0, 0.0};

    LaneletInfo current_lanelet;
    current_lanelet.id = "lanelet_1001";
    current_lanelet.speed_limit_mps = 13.8;

    std::vector<std::string> route = {"lanelet_1001"};
    
    // Obstacle critical close (3 meters ahead)
    DetectedObject obs;
    obs.id = 99;
    obs.position = {23.0, 0.0, 0.0};
    obs.velocity = {0.0, 0.0, 0.0};

    std::vector<DetectedObject> obstacles = {obs};

    auto decision = behavior_planner_.plan(
        ego_pose, ego_vel, route, current_lanelet, obstacles, TrafficLightState::Green);
    
    EXPECT_EQ(decision.state, BehaviorState::EmergencyStop);
    EXPECT_NEAR(decision.target_speed, 0.0, 0.01);
    EXPECT_TRUE(decision.emergency_braking);
}

// ============================================================================
// 3. Local Motion Planner Tests
// ============================================================================

TEST_F(PlanningSubsystemTest, TestMotionPlannerCruisingTrajectory) {
    KinematicState ego_state;
    ego_state.pose.position = {0.0, 0.0, 0.0};
    ego_state.pose.orientation = Quat::Identity();
    ego_state.velocity = {10.0, 0.0, 0.0};
    ego_state.acceleration = {0.0, 0.0, 0.0};

    BehaviorDecision decision;
    decision.state = BehaviorState::Cruise;
    decision.target_speed = 12.0;

    LaneletInfo current_lanelet;
    current_lanelet.distance_to_centerline = 0.0;
    current_lanelet.speed_limit_mps = 13.8;

    std::vector<DetectedObject> obstacles;

    auto trajectory = motion_planner_.plan_trajectory(
        ego_state, decision, current_lanelet, obstacles, 3.0, 0.1);

    ASSERT_FALSE(trajectory.points.empty());
    EXPECT_FALSE(trajectory.is_fallback);

    // Initial heading should align with x-axis
    EXPECT_NEAR(trajectory.points.front().heading, 0.0, 0.01);
    // Speed should transition toward target speed of 12.0
    EXPECT_NEAR(trajectory.points.back().speed, 12.0, 0.1);
}

TEST_F(PlanningSubsystemTest, TestMotionPlannerQuinticLateralSmoother) {
    KinematicState ego_state;
    // Ego is drifted 1.0 meter off-center laterally to the left
    ego_state.pose.position = {0.0, 1.0, 0.0};
    ego_state.pose.orientation = Quat::Identity();
    ego_state.velocity = {10.0, 0.0, 0.0};
    ego_state.acceleration = {0.0, 0.0, 0.0};

    BehaviorDecision decision;
    decision.state = BehaviorState::Cruise;
    decision.target_speed = 10.0;

    LaneletInfo current_lanelet;
    // Current distance to centerline is 1.0m
    current_lanelet.distance_to_centerline = 1.0;
    current_lanelet.speed_limit_mps = 13.8;

    std::vector<DetectedObject> obstacles;

    auto trajectory = motion_planner_.plan_trajectory(
        ego_state, decision, current_lanelet, obstacles, 4.0, 0.1);

    ASSERT_FALSE(trajectory.points.empty());
    // Starting waypoint lateral offset
    EXPECT_NEAR(trajectory.points.front().position.y, 1.0, 0.01);
    // Over time (T = 2.0s), the spline should bring vehicle back to centerline y = 0.0
    EXPECT_NEAR(trajectory.points.back().position.y, 0.0, 0.05);

    // Validate that headings diverge and recover smoothly
    EXPECT_NEAR(trajectory.points.back().heading, 0.0, 0.05);
}

TEST_F(PlanningSubsystemTest, TestMotionPlannerEmergencyFallback) {
    KinematicState ego_state;
    ego_state.pose.position = {0.0, 0.0, 0.0};
    ego_state.pose.orientation = Quat::Identity();
    ego_state.velocity = {10.0, 0.0, 0.0};

    auto trajectory = motion_planner_.generate_fallback_trajectory(ego_state, 3.0, 0.1);

    ASSERT_FALSE(trajectory.points.empty());
    EXPECT_TRUE(trajectory.is_fallback);

    // Check decelerating speed
    EXPECT_NEAR(trajectory.points.back().speed, 0.0, 0.01);
    // Acceleration must reflect emergency deceleration (comfort/braking limit)
    EXPECT_LE(trajectory.points[1].acceleration, -3.0);
}

TEST_F(PlanningSubsystemTest, TestMotionPlannerProactiveCollisionChecking) {
    KinematicState ego_state;
    ego_state.pose.position = {0.0, 0.0, 0.0};
    ego_state.pose.orientation = Quat::Identity();
    ego_state.velocity = {10.0, 0.0, 0.0};

    BehaviorDecision decision;
    decision.state = BehaviorState::Cruise;
    decision.target_speed = 10.0;

    LaneletInfo current_lanelet;
    current_lanelet.distance_to_centerline = 0.0;
    current_lanelet.speed_limit_mps = 13.8;

    // Obstacle directly in the path ahead at 15 meters
    DetectedObject obs;
    obs.id = 55;
    obs.position = {15.0, 0.0, 0.0};
    obs.velocity = {0.0, 0.0, 0.0};
    std::vector<DetectedObject> obstacles = {obs};

    // Plan trajectory
    auto trajectory = motion_planner_.plan_trajectory(
        ego_state, decision, current_lanelet, obstacles, 3.0, 0.1);

    ASSERT_FALSE(trajectory.points.empty());
    // Proactive collision checker must detect the threat and switch to a safe fallback stop path
    EXPECT_TRUE(trajectory.is_fallback);
    EXPECT_NEAR(trajectory.points.back().speed, 0.0, 0.01);
}
