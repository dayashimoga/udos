#include "uados/simulation/scenario_engine.hpp"
#include "uados/simulation/replay_system.hpp"

#include <gtest/gtest.h>
#include <chrono>

using namespace uados;
using namespace uados::simulation;

class SimulationSubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        uados::core::Config mock_config;
        
        ASSERT_EQ(scenario_engine_.init(mock_config), Status::Ok);
        ASSERT_EQ(replay_system_.init(mock_config), Status::Ok);

        ASSERT_EQ(scenario_engine_.start(), Status::Ok);
        ASSERT_EQ(replay_system_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(scenario_engine_.stop(), Status::Ok);
        EXPECT_EQ(replay_system_.stop(), Status::Ok);
    }

    ScenarioEngine scenario_engine_;
    ReplaySystem replay_system_;
};

// ============================================================================
// 1. Scenario Engine Tests
// ============================================================================

TEST_F(SimulationSubsystemTest, TestScenarioLoadingAndBatchStepping) {
    // Obstacle 1: 30.0m directly ahead
    DetectedObject target;
    target.id = 1;
    target.object_class = ObjectClass::Car;
    target.position = {30.0, 0.0, 0.0};
    target.velocity = {5.0, 0.0, 0.0}; // moves at 5m/s

    std::vector<DetectedObject> obstacles = {target};

    // Load Scenario starting at origin at 10 m/s
    scenario_engine_.load_scenario(0.0, 0.0, 10.0, obstacles);

    // Step physics loop for 1.0 second (dt = 0.1s, 10 cycles)
    for (int i = 0; i < 10; ++i) {
        scenario_engine_.step(0.0, 0.0, 0.1);
    }

    auto metrics = scenario_engine_.get_metrics();
    auto ego_state = scenario_engine_.get_vehicle_twin().get_state();

    // Verify ego speed: v = 10.0 m/s (no accel)
    EXPECT_NEAR(ego_state.velocity.vx, 10.0, 0.01);
    // Ego advanced: s = v * t = 10.0 * 1.0 = 10.0m
    EXPECT_NEAR(ego_state.position.x, 10.0, 0.05);

    // Verify traffic agent propagated: x_final = 30.0 + 5.0 * 1.0 = 35.0m
    auto agents = scenario_engine_.get_traffic_agents();
    ASSERT_EQ(agents.size(), 1);
    EXPECT_NEAR(agents[0].position.x, 35.0, 0.05);

    // No collision should be triggered (min distance was 30m down to 25m)
    EXPECT_FALSE(metrics.collision_occurred);
    EXPECT_NEAR(metrics.min_obstacle_distance, 25.0, 0.5);
    EXPECT_NEAR(metrics.total_simulated_time, 1.0, 0.01);
}

TEST_F(SimulationSubsystemTest, TestScenarioCollisionDetection) {
    // Dynamic obstacle directly in the path (very close, 5.0m ahead)
    DetectedObject target;
    target.id = 2;
    target.object_class = ObjectClass::Pedestrian;
    target.position = {5.0, 0.0, 0.0};
    target.velocity = {0.0, 0.0, 0.0}; // static

    std::vector<DetectedObject> obstacles = {target};

    scenario_engine_.load_scenario(0.0, 0.0, 10.0, obstacles);

    // Step simulation: 0.4 seconds -> advanced 4.0 meters -> ego is within 1.0m of pedestrian -> COLLISION!
    for (int i = 0; i < 4; ++i) {
        scenario_engine_.step(0.0, 0.0, 0.1);
    }

    auto metrics = scenario_engine_.get_metrics();
    EXPECT_TRUE(metrics.collision_occurred);
    EXPECT_GE(metrics.safety_violations, 1);
}

// ============================================================================
// 2. Replay System Serialization and Query Tests
// ============================================================================

TEST_F(SimulationSubsystemTest, TestReplaySerializationAndFrameRecovery) {
    // 1. Record frame 1 at t = 1.0s
    VehicleState ego1;
    ego1.position = {10.0, 0.0, 0.0};
    ego1.velocity = {5.0, 0.0, 0.0};
    ego1.orientation = Quat::Identity();

    DetectedObject obs1;
    obs1.id = 505;
    obs1.object_class = ObjectClass::Car;
    obs1.position = {25.0, 0.0, 0.0};
    obs1.velocity = {0.0, 0.0, 0.0};

    replay_system_.record_frame(1.0, ego1, {obs1});

    // 2. Record frame 2 at t = 2.0s
    VehicleState ego2;
    ego2.position = {15.0, 0.0, 0.0};
    ego2.velocity = {5.0, 0.0, 0.0};
    ego2.orientation = Quat::Identity();

    replay_system_.record_frame(2.0, ego2, {obs1});

    EXPECT_EQ(replay_system_.get_frame_count(), 2);

    // 3. Serialize to JSON log
    std::string json_log = replay_system_.serialize_log();
    
    // JSON must contain structured field keywords
    EXPECT_NE(json_log.find("time_s"), std::string::npos);
    EXPECT_NE(json_log.find("ego_x"), std::string::npos);
    EXPECT_NE(json_log.find("obstacles"), std::string::npos);

    // 4. Clear and deserialize log
    replay_system_.clear();
    EXPECT_EQ(replay_system_.get_frame_count(), 0);

    ASSERT_EQ(replay_system_.load_log(json_log), Status::Ok);
    EXPECT_EQ(replay_system_.get_frame_count(), 2);

    // 5. Query time step lookup (e.g. searching closest match for t = 1.2s -> should return frame 1 at 1.0s)
    ReplayFrame recovered_frame;
    ASSERT_TRUE(replay_system_.get_frame(1.2, recovered_frame));

    EXPECT_NEAR(recovered_frame.timestamp_s, 1.0, 0.01);
    EXPECT_NEAR(recovered_frame.ego_state.position.x, 10.0, 0.05);
    EXPECT_NEAR(recovered_frame.ego_state.velocity.vx, 5.0, 0.01);
    
    ASSERT_EQ(recovered_frame.obstacles.size(), 1);
    EXPECT_EQ(recovered_frame.obstacles[0].id, 505);
    EXPECT_NEAR(recovered_frame.obstacles[0].position.x, 25.0, 0.05);
}
