#include "uados/validation/automated_validator.hpp"
#include "uados/validation/fault_injector.hpp"
#include "uados/safety/safety_monitor.hpp"

#include <gtest/gtest.h>
#include <chrono>

using namespace uados;
using namespace uados::validation;
using namespace uados::safety;

class ValidationSubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        uados::core::Config mock_config;
        
        ASSERT_EQ(automated_validator_.init(mock_config), Status::Ok);
        ASSERT_EQ(fault_injector_.init(mock_config), Status::Ok);

        ASSERT_EQ(automated_validator_.start(), Status::Ok);
        ASSERT_EQ(fault_injector_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(automated_validator_.stop(), Status::Ok);
        EXPECT_EQ(fault_injector_.stop(), Status::Ok);
    }

    AutomatedValidator automated_validator_;
    FaultInjector fault_injector_;
};

// ============================================================================
// 1. Automated Validator Tests
// ============================================================================

TEST_F(ValidationSubsystemTest, TestAutomatedSuiteExecutionAndReporting) {
    // Execute compliance scenarios in batch
    automated_validator_.run_validation_suite();

    auto results = automated_validator_.get_results();
    
    // There must be 3 scenarios executed in the scorecard
    ASSERT_EQ(results.size(), 3);
    
    // Verify each test case has passed successfully
    EXPECT_EQ(results[0].name, "TC-VAL-001: Nominal Cruise Linelet Tracking");
    EXPECT_TRUE(results[0].passed);

    EXPECT_EQ(results[1].name, "TC-VAL-002: Behavior Stop Line Halting");
    EXPECT_TRUE(results[1].passed);

    EXPECT_EQ(results[2].name, "TC-VAL-003: Critical Envelope Anomaly Override");
    EXPECT_TRUE(results[2].passed);

    // Compile validation evidence report
    std::string report = automated_validator_.compile_evidence_report();

    // Verify Markdown structure keywords exist in report string
    EXPECT_NE(report.find("# UADOS — Automated Compliance Validation Report"), std::string::npos);
    EXPECT_NE(report.find("TC-VAL-001"), std::string::npos);
    EXPECT_NE(report.find("✅ **PASSED**"), std::string::npos);
    EXPECT_NE(report.find("System Pass Rate"), std::string::npos);
}

// ============================================================================
// 2. Fault Injector Tests
// ============================================================================

TEST_F(ValidationSubsystemTest, TestFaultInjectionSpeedSpike) {
    VehicleState state;
    state.velocity.vx = 10.0;

    // Inject speed spike of +5.0 m/s
    fault_injector_.inject_speed_spike(state, 5.0);

    EXPECT_NEAR(state.velocity.vx, 15.0, 0.01);
}

TEST_F(ValidationSubsystemTest, TestFaultInjectionLateralDriftAndFailSafe) {
    // Set up nominal state
    VehicleState state;
    state.position = {0.0, 0.0, 0.0};
    state.velocity = {10.0, 0.0, 0.0};
    state.orientation = Quat::Identity();

    uados::localization::LaneletInfo lanelet;
    lanelet.speed_limit_mps = 13.8;

    VehicleCommand command;
    command.throttle = 0.5;
    command.brake = 0.0;
    command.steering_angle = 0.0;

    SafetyMonitor monitor;
    uados::core::Config mock_config;
    ASSERT_EQ(monitor.init(mock_config), Status::Ok);
    ASSERT_EQ(monitor.start(), Status::Ok);

    // Audit under nominal conditions -> Status::Ok
    EXPECT_EQ(monitor.audit_safety(state, lanelet, 0.0, 0.0, command), Status::Ok);
    EXPECT_FALSE(command.emergency_stop);

    // Inject lateral drift offset (+2.0m, limit is 1.8m -> breach!)
    fault_injector_.inject_lateral_drift(state, 2.0);

    // Audit under faulty conditions -> Safety Monitor must trigger emergency stop override
    Status audit_status = monitor.audit_safety(state, lanelet, state.position.y, 0.0, command);

    EXPECT_EQ(audit_status, Status::Error);
    EXPECT_TRUE(command.emergency_stop);
    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 1.0, 0.01); // full braking override

    EXPECT_EQ(monitor.stop(), Status::Ok);
}

TEST_F(ValidationSubsystemTest, TestFaultInjectionBOSActuatorConflict) {
    VehicleCommand command;
    command.throttle = 0.0;
    command.brake = 0.0;

    // Inject simultaneous full throttle and brake conflict command
    fault_injector_.inject_bos_fault(command);

    EXPECT_NEAR(command.throttle, 1.0, 0.01);
    EXPECT_NEAR(command.brake, 1.0, 0.01);

    // Feed command into Safety Monitor to verify BOS throttle clamping overrides
    SafetyMonitor monitor;
    uados::core::Config mock_config;
    ASSERT_EQ(monitor.init(mock_config), Status::Ok);
    ASSERT_EQ(monitor.start(), Status::Ok);

    VehicleState state;
    state.velocity.vx = 10.0;
    uados::localization::LaneletInfo lanelet;

    Status audit_status = monitor.audit_safety(state, lanelet, 0.0, 0.0, command);

    EXPECT_EQ(audit_status, Status::Ok);
    // BOS interlock must clamp throttle back to 0.0!
    EXPECT_NEAR(command.throttle, 0.0, 0.01);
    EXPECT_NEAR(command.brake, 1.0, 0.01);

    EXPECT_EQ(monitor.stop(), Status::Ok);
}
