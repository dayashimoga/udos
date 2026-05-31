#include "uados/control/stanley_controller.hpp"
#include "uados/control/longitudinal_controller.hpp"
#include "uados/control/control_loop.hpp"

#include <gtest/gtest.h>
#include <chrono>

using namespace uados;
using namespace uados::control;

class ControlSubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        uados::core::Config mock_config;
        
        ASSERT_EQ(control_loop_.init(mock_config), Status::Ok);
        ASSERT_EQ(control_loop_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(control_loop_.stop(), Status::Ok);
    }

    ControlLoop control_loop_;
};

// ============================================================================
// 1. Stanley Lateral Controller Tests
// ============================================================================

TEST_F(ControlSubsystemTest, TestStanleySteeringHeadingCorrection) {
    StanleyController controller;
    controller.configure(1.5, 1.0, 0.70);

    // Ego has heading error (drifts 0.1 rad to the left of a straight path)
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    
    // Euler angles roll=0, pitch=0, yaw=0.1
    state.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                        Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                        Eigen::AngleAxisd(0.1, Eigen::Vector3d::UnitZ());

    // Path is straight along x-axis (y=0, heading=0, curvature=0)
    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {10.0, 0.0, 0.0};
    pt.heading = 0.0;
    pt.curvature = 0.0;
    path.points.push_back(pt);

    double cte = 0.0, he = 0.0;
    double steer = controller.calculate_steering(state, path, cte, he);

    // Steer command should react to counteract heading drift
    EXPECT_LT(steer, 0.0);
    EXPECT_NEAR(he, -0.1, 0.01);
}

TEST_F(ControlSubsystemTest, TestStanleySteeringLateralCorrection) {
    StanleyController controller;
    controller.configure(1.5, 1.0, 0.70);

    // Ego is translated laterally to the left of the path by 0.5m
    VehicleState state;
    state.position = {0.0, 0.5, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {10.0, 0.0, 0.0};
    pt.heading = 0.0;
    pt.curvature = 0.0;
    path.points.push_back(pt);

    double cte = 0.0, he = 0.0;
    double steer = controller.calculate_steering(state, path, cte, he);

    // Steer command should react to correct cross-track error to the right
    EXPECT_LT(steer, 0.0);
    EXPECT_NEAR(cte, 0.5, 0.05); // CTE front axle projection is approx 0.5m
}

TEST_F(ControlSubsystemTest, TestStanleySteeringSaturationLimits) {
    StanleyController controller;
    controller.configure(5.0, 1.0, 0.50); // small max limit

    // Ego has massive lateral error (drifts 10 meters off path)
    VehicleState state;
    state.position = {0.0, 10.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {10.0, 0.0, 0.0};
    pt.heading = 0.0;
    pt.curvature = 0.0;
    path.points.push_back(pt);

    double cte = 0.0, he = 0.0;
    double steer = controller.calculate_steering(state, path, cte, he);

    // Steering should saturate perfectly at the configured limit
    EXPECT_NEAR(steer, -0.50, 0.01);
}

// ============================================================================
// 2. Longitudinal PID Controller Tests
// ============================================================================

TEST_F(ControlSubsystemTest, TestLongitudinalThrottleCommand) {
    LongitudinalController controller;
    controller.configure(1.5, 0.1, 0.05, 3.0, 5.0);

    // Ego is moving at 8.0 m/s, target speed is 10.0 m/s -> Accelerate
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {8.0, 0.0, 0.0};

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {0.0, 0.0, 0.0};
    pt.speed = 10.0;
    pt.acceleration = 0.5; // positive FF acceleration
    path.points.push_back(pt);

    double speed_err = 0.0, throttle = 0.0, brake = 0.0;
    controller.calculate_longitudinal(state, path, 0.1, speed_err, throttle, brake);

    EXPECT_NEAR(speed_err, 2.0, 0.01);
    EXPECT_GT(throttle, 0.0);
    EXPECT_NEAR(brake, 0.0, 0.01);
}

TEST_F(ControlSubsystemTest, TestLongitudinalBrakingCommand) {
    LongitudinalController controller;
    controller.configure(1.5, 0.1, 0.05, 3.0, 5.0);

    // Ego is moving at 12.0 m/s, target speed is 10.0 m/s -> Decelerate
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {12.0, 0.0, 0.0};

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {0.0, 0.0, 0.0};
    pt.speed = 10.0;
    pt.acceleration = -0.5; // negative FF acceleration
    path.points.push_back(pt);

    double speed_err = 0.0, throttle = 0.0, brake = 0.0;
    controller.calculate_longitudinal(state, path, 0.1, speed_err, throttle, brake);

    EXPECT_NEAR(speed_err, -2.0, 0.01);
    EXPECT_NEAR(throttle, 0.0, 0.01);
    EXPECT_GT(brake, 0.0);
}

// ============================================================================
// 3. Control Loop Orchestrator Tests
// ============================================================================

TEST_F(ControlSubsystemTest, TestControlLoopSafetyGatesCTE) {
    // Normal operation state
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    // Straight trajectory waypoints
    Trajectory path;
    TrajectoryPoint pt1; pt1.position = {5.0, 0.0, 0.0}; pt1.speed = 10.0;
    TrajectoryPoint pt2; pt2.position = {10.0, 0.0, 0.0}; pt2.speed = 10.0;
    path.points = {pt1, pt2};

    auto cmd = control_loop_.update(state, path, 0.01);

    // Under normal track errors, emergency stop is inactive
    EXPECT_FALSE(cmd.emergency_stop);
    EXPECT_NEAR(cmd.brake, 0.0, 0.1);
    EXPECT_EQ(control_loop_.health(), HealthStatus::Healthy);

    // Trigger Lateral Error Safety Override Gate (Ego drifts 3.0m off course)
    state.position = {0.0, 3.0, 0.0};
    auto emergency_cmd = control_loop_.update(state, path, 0.01);

    // Safety gate should enforce high priority safety stop override
    EXPECT_TRUE(emergency_cmd.emergency_stop);
    EXPECT_NEAR(emergency_cmd.throttle, 0.0, 0.01);
    EXPECT_NEAR(emergency_cmd.brake, 1.0, 0.01); // full braking clamp
    EXPECT_EQ(control_loop_.health(), HealthStatus::Degraded);
}

TEST_F(ControlSubsystemTest, TestControlLoopFallbackTrajectoryDecel) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {5.0, 0.0, 0.0};
    pt.speed = 5.0;
    path.points = {pt};
    path.is_fallback = true; // Fallback deceleration trajectory active

    auto cmd = control_loop_.update(state, path, 0.01);

    // Orchestrator must enforce high braking torque on fallback trajectories
    EXPECT_FALSE(cmd.emergency_stop); // fallback stop is a planned stop, not safety crash override
    EXPECT_NEAR(cmd.throttle, 0.0, 0.01);
    EXPECT_GE(cmd.brake, 0.8);
}

// ============================================================================
// 4. Edge Case & Boundary Tests
// ============================================================================

TEST_F(ControlSubsystemTest, TestStanleyZeroVelocityEpsilonGuard) {
    // At v=0 the Stanley denominator (v+epsilon) must not cause division-by-zero
    StanleyController controller;
    controller.configure(1.5, 1.0, 0.70);

    VehicleState state;
    state.position = {0.0, 1.0, 0.0}; // 1m lateral offset
    state.velocity = {0.0, 0.0, 0.0}; // ZERO velocity
    state.orientation = Quat::Identity();

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {5.0, 0.0, 0.0};
    pt.heading = 0.0;
    pt.curvature = 0.0;
    path.points.push_back(pt);

    double cte = 0.0, he = 0.0;
    double steer = controller.calculate_steering(state, path, cte, he);

    // Result must be finite (no NaN/Inf from division by zero)
    EXPECT_TRUE(std::isfinite(steer));
    EXPECT_TRUE(std::isfinite(cte));
    EXPECT_TRUE(std::isfinite(he));
}

TEST_F(ControlSubsystemTest, TestControlLoopEmptyTrajectorySafeStop) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    Trajectory empty_path; // No waypoints at all

    auto cmd = control_loop_.update(state, empty_path, 0.01);

    // Empty path must produce a comfortable safe-stop, not a crash
    EXPECT_NEAR(cmd.steering_angle, 0.0, 0.01);
    EXPECT_NEAR(cmd.throttle, 0.0, 0.01);
    EXPECT_NEAR(cmd.brake, 1.0, 0.01);
    EXPECT_FALSE(cmd.emergency_stop); // safe stop, not an emergency override
}

TEST_F(ControlSubsystemTest, TestStanleyHeadingWrapAroundPi) {
    // Test heading error normalization when yaw wraps across ±π boundary
    StanleyController controller;
    controller.configure(1.0, 0.0, 1.0);

    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {5.0, 0.0, 0.0};
    // Ego heading at nearly +π (facing backwards-left)
    state.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                        Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                        Eigen::AngleAxisd(3.0, Eigen::Vector3d::UnitZ());

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {-5.0, 0.0, 0.0};
    pt.heading = -3.0; // Path heading nearly -π (same physical direction, opposite sign)
    pt.curvature = 0.0;
    path.points.push_back(pt);

    double cte = 0.0, he = 0.0;
    double steer = controller.calculate_steering(state, path, cte, he);

    // Heading error must be normalized to [-π, π], not produce a 2π discontinuity
    EXPECT_TRUE(std::isfinite(steer));
    EXPECT_LE(std::abs(he), M_PI + 0.01);
}

TEST_F(ControlSubsystemTest, TestLongitudinalIntegralWindupClamp) {
    LongitudinalController controller;
    controller.configure(0.5, 1.0, 0.0, 3.0, 5.0); // high ki to trigger windup

    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {0.0, 0.0, 0.0}; // stopped

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {0.0, 0.0, 0.0};
    pt.speed = 20.0; // large speed error to provoke integral accumulation
    pt.acceleration = 0.0;
    path.points.push_back(pt);

    // Rapidly accumulate integral error over many cycles
    double throttle = 0.0, brake = 0.0, speed_err = 0.0;
    for (int i = 0; i < 100; ++i) {
        controller.calculate_longitudinal(state, path, 0.1, speed_err, throttle, brake);
    }

    // Output must be clamped to [0, 1], never exceed despite massive integral
    EXPECT_LE(throttle, 1.0);
    EXPECT_GE(throttle, 0.0);
    EXPECT_GE(brake, 0.0);
    EXPECT_LE(brake, 1.0);
}

TEST_F(ControlSubsystemTest, TestControlLoopStartStopLifecycleIdempotent) {
    // Calling start() twice and stop() twice should be safe (idempotent)
    EXPECT_EQ(control_loop_.start(), Status::Ok);
    EXPECT_EQ(control_loop_.start(), Status::Ok);
    EXPECT_EQ(control_loop_.stop(), Status::Ok);
    EXPECT_EQ(control_loop_.stop(), Status::Ok);
}

TEST_F(ControlSubsystemTest, TestControlLoopUpdateWhileStoppedReturnsEmpty) {
    control_loop_.stop();

    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {5.0, 0.0, 0.0};
    pt.speed = 10.0;
    path.points.push_back(pt);

    auto cmd = control_loop_.update(state, path, 0.01);

    // Stopped controller should not produce active commands
    EXPECT_NEAR(cmd.throttle, 0.0, 0.01);
    EXPECT_NEAR(cmd.brake, 0.0, 0.01);
    EXPECT_NEAR(cmd.steering_angle, 0.0, 0.01);
}

TEST_F(ControlSubsystemTest, TestStanleyLargeCurvatureFeedforward) {
    StanleyController controller;
    controller.configure(1.0, 2.0, 0.70); // high feedforward gain

    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    Trajectory path;
    TrajectoryPoint pt;
    pt.position = {5.0, 0.0, 0.0};
    pt.heading = 0.0;
    pt.curvature = 0.3; // sharp curve
    path.points.push_back(pt);

    double cte = 0.0, he = 0.0;
    double steer = controller.calculate_steering(state, path, cte, he);

    // Feedforward should add positive steering for positive curvature
    EXPECT_GT(steer, 0.0);
    // But still clamped within mechanical limits
    EXPECT_LE(std::abs(steer), 0.70 + 0.01);
}
