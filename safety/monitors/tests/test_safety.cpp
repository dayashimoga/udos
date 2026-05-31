#include "uados/safety/safety_monitor.hpp"
#include "uados/safety/emergency_response_system.hpp"
#include "uados/localization/hdmap_engine.hpp"

#include <gtest/gtest.h>
#include <chrono>

using namespace uados;
using namespace uados::safety;
using namespace uados::localization;

class SafetySubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        uados::core::Config mock_config;
        
        ASSERT_EQ(safety_monitor_.init(mock_config), Status::Ok);
        ASSERT_EQ(emergency_system_.init(mock_config), Status::Ok);

        ASSERT_EQ(safety_monitor_.start(), Status::Ok);
        ASSERT_EQ(emergency_system_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(safety_monitor_.stop(), Status::Ok);
        EXPECT_EQ(emergency_system_.stop(), Status::Ok);
    }

    SafetyMonitor safety_monitor_;
    EmergencyResponseSystem emergency_system_;
};

// ============================================================================
// 1. Safety Monitor Invariant Tests
// ============================================================================

TEST_F(SafetySubsystemTest, TestBrakeOverrideSystemInterlock) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    // Conflicting simultaneous full throttle + brake command
    VehicleCommand command;
    command.throttle = 0.8;
    command.brake = 0.5;
    command.steering_angle = 0.1;

    // Audit safety loops
    Status status = safety_monitor_.audit_safety(state, lanelet, 0.0, 0.0, command);

    EXPECT_EQ(status, Status::Ok);
    // BOS interlock must clamp throttle to 0.0 immediately
    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 0.5, 0.01);

    auto violations = safety_monitor_.get_violations();
    ASSERT_GE(violations.size(), 1);
    EXPECT_EQ(violations[0].rule_name, "Brake Override System Interlock");
    EXPECT_EQ(violations[0].severity, SafetySeverity::Warning);
}

TEST_F(SafetySubsystemTest, TestSteeringSaturationClamping) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    // Extreme steer requested (1.2 rad, mechanical limit is ~0.72 rad)
    VehicleCommand command;
    command.steering_angle = 1.2;
    command.throttle = 0.5;
    command.brake = 0.0;

    Status status = safety_monitor_.audit_safety(state, lanelet, 0.0, 0.0, command);

    EXPECT_EQ(status, Status::Ok);
    // Steering must be clamped perfectly to mechanical safeguard bounds
    EXPECT_NEAR(command.steering_angle, 0.72, 0.01);

    auto violations = safety_monitor_.get_violations();
    ASSERT_GE(violations.size(), 1);
    EXPECT_EQ(violations[0].rule_name, "Mechanical Steering Saturation Guard");
}

TEST_F(SafetySubsystemTest, TestSpeedLimitAuditingBreach) {
    // Ego vehicle is moving at 17.0 m/s (limit is 13.8, speed buffer is 2.0 -> breach!)
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {17.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    VehicleCommand command;
    command.throttle = 0.5;
    command.brake = 0.0;

    Status status = safety_monitor_.audit_safety(state, lanelet, 0.0, 0.0, command);

    EXPECT_EQ(status, Status::Ok); // Speed limit breach triggers warning, not immediate emergency shutdown
    
    auto violations = safety_monitor_.get_violations();
    ASSERT_GE(violations.size(), 1);
    EXPECT_EQ(violations[0].rule_name, "Speed Limit Boundary");
    EXPECT_EQ(violations[0].severity, SafetySeverity::Warning);
}

TEST_F(SafetySubsystemTest, TestODDTrackingBoundaryBreach) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    VehicleCommand command;
    command.throttle = 0.5;
    command.brake = 0.0;

    // Lateral error is 2.0 meters (ODD boundary limit is 1.8m -> breach!)
    Status status = safety_monitor_.audit_safety(state, lanelet, 2.0, 0.0, command);

    // Should return Error to report safety envelope breach
    EXPECT_EQ(status, Status::Error);
    // Command must be overridden with emergency stop values
    EXPECT_TRUE(command.emergency_stop);
    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 1.0, 0.01);
    EXPECT_EQ(safety_monitor_.health(), HealthStatus::Unhealthy);

    auto violations = safety_monitor_.get_violations();
    ASSERT_GE(violations.size(), 1);
    EXPECT_EQ(violations[0].rule_name, "ODD Tracking Boundary Breach");
    EXPECT_EQ(violations[0].severity, SafetySeverity::Emergency);
}

// ============================================================================
// 2. Emergency Response FSM Tests
// ============================================================================

TEST_F(SafetySubsystemTest, TestEmergencyFSMStateTransitions) {
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::Normal);

    // 1. Trigger MRC safe-stop pull over
    emergency_system_.trigger_mrc();
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::ActiveMRC);

    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {5.0, 0.0, 0.0}; // still moving at 5m/s

    VehicleCommand command;
    command.throttle = 0.5;
    command.brake = 0.0;

    // 2. Execute active MRC deceleration
    emergency_system_.execute_mrc(state, command, 0.1);

    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 0.6, 0.01); // active MRC braking
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::ActiveMRC);

    // 3. Vehicle has completed the halt (speed feedback is 0.0m/s)
    state.velocity = {0.0, 0.0, 0.0};
    emergency_system_.execute_mrc(state, command, 0.1);

    // ERS must transition to SafeState, locking gear in Park
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::SafeState);
    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 1.0, 0.01); // park lock brake pressure
    EXPECT_EQ(command.gear, GearPosition::Park);
    EXPECT_TRUE(command.emergency_stop); // hazards active

    // 4. Test recovery reset
    emergency_system_.reset_nominal();
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::Normal);
}

// ============================================================================
// 3. Edge Case & Boundary Tests
// ============================================================================

TEST_F(SafetySubsystemTest, TestNominalPassProducesNoViolations) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    VehicleCommand command;
    command.throttle = 0.3;
    command.brake = 0.0;
    command.steering_angle = 0.1;
    command.emergency_stop = false;

    // All parameters within safe bounds
    Status status = safety_monitor_.audit_safety(state, lanelet, 0.5, 0.1, command);

    EXPECT_EQ(status, Status::Ok);
    EXPECT_FALSE(command.emergency_stop);
    EXPECT_NEAR(command.throttle, 0.3, 0.01); // throttle unchanged
    EXPECT_EQ(safety_monitor_.health(), HealthStatus::Healthy);
}

TEST_F(SafetySubsystemTest, TestHeadingErrorODDBoundaryBreach) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    VehicleCommand command;
    command.throttle = 0.3;
    command.brake = 0.0;

    // Heading error exceeds 0.8 rad limit
    Status status = safety_monitor_.audit_safety(state, lanelet, 0.0, 1.2, command);

    EXPECT_EQ(status, Status::Error);
    EXPECT_TRUE(command.emergency_stop);
    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 1.0, 0.01);
}

TEST_F(SafetySubsystemTest, TestDoubleMRCTriggerIdempotent) {
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::Normal);

    // Triggering MRC twice should not corrupt state
    emergency_system_.trigger_mrc();
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::ActiveMRC);

    emergency_system_.trigger_mrc(); // second trigger
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::ActiveMRC);

    // Should still be able to reset cleanly
    emergency_system_.reset_nominal();
    EXPECT_EQ(emergency_system_.get_emergency_state(), EmergencyState::Normal);
}

TEST_F(SafetySubsystemTest, TestRapidViolationBurstStability) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    // Generate rapid violation bursts (100 calls with conflicting throttle+brake)
    for (int i = 0; i < 100; ++i) {
        VehicleCommand command;
        command.throttle = 0.8;
        command.brake = 0.5;
        command.steering_angle = 0.1;
        safety_monitor_.audit_safety(state, lanelet, 0.0, 0.0, command);
    }

    // System must not crash or corrupt internal state under rapid violation load
    auto violations = safety_monitor_.get_violations();
    EXPECT_GE(violations.size(), 100u);
    EXPECT_EQ(safety_monitor_.health(), HealthStatus::Healthy);
}

TEST_F(SafetySubsystemTest, TestZeroSpeedLimitEdgeCase) {
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {1.0, 0.0, 0.0}; // moving

    LaneletInfo lanelet;
    lanelet.speed_limit_mps = 0.0; // degenerate: zero speed limit

    VehicleCommand command;
    command.throttle = 0.3;
    command.brake = 0.0;

    // Any positive speed exceeds a zero speed limit
    Status status = safety_monitor_.audit_safety(state, lanelet, 0.0, 0.0, command);

    EXPECT_EQ(status, Status::Ok); // Warning-level, not emergency
    auto violations = safety_monitor_.get_violations();
    bool found_speed_violation = false;
    for (const auto& v : violations) {
        if (v.rule_name == "Speed Limit Boundary") found_speed_violation = true;
    }
    EXPECT_TRUE(found_speed_violation);
}

