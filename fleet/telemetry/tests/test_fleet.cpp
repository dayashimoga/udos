#include "uados/fleet/fleet_telemetry.hpp"
#include "uados/fleet/ota_manager.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <chrono>

using namespace uados;
using namespace uados::fleet;

class FleetSubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        uados::core::Config mock_config;
        
        ASSERT_EQ(fleet_telemetry_.init(mock_config), Status::Ok);
        ASSERT_EQ(ota_manager_.init(mock_config), Status::Ok);

        ASSERT_EQ(fleet_telemetry_.start(), Status::Ok);
        ASSERT_EQ(ota_manager_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(fleet_telemetry_.stop(), Status::Ok);
        EXPECT_EQ(ota_manager_.stop(), Status::Ok);
    }

    std::string calculate_djb2_hash(const std::string& str) {
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<unsigned long>(c);
        }
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }

    FleetTelemetry fleet_telemetry_;
    OTAManager ota_manager_;
};

// ============================================================================
// 1. Fleet Telemetry Packaging Tests
// ============================================================================

TEST_F(FleetSubsystemTest, TestFleetTelemetryPackagingAndTransmission) {
    VehicleState state;
    state.position = {40.0, -1.5, 0.0};
    state.velocity = {12.5, 0.0, 0.0};

    // Package telemetry into JSON string
    std::string json_packet = fleet_telemetry_.package_telemetry(state, 0.05, 0.01, false);

    ASSERT_FALSE(json_packet.empty());

    // Parse back to verify JSON parameters
    auto packet = nlohmann::json::parse(json_packet);
    
    EXPECT_EQ(packet["vehicle_id"], "uados-ego-carla-001");
    EXPECT_NE(packet["timestamp"], "");
    
    // Verify kinematics structure
    EXPECT_NEAR(packet["kinematics"]["x"].get<double>(), 40.0, 0.01);
    EXPECT_NEAR(packet["kinematics"]["y"].get<double>(), -1.5, 0.01);
    EXPECT_NEAR(packet["kinematics"]["vx"].get<double>(), 12.5, 0.01);

    // Verify diagnostics structure
    EXPECT_NEAR(packet["diagnostics"]["cross_track_error"].get<double>(), 0.05, 0.01);
    EXPECT_NEAR(packet["diagnostics"]["heading_error"].get<double>(), 0.01, 0.01);
    EXPECT_FALSE(packet["diagnostics"]["emergency_active"].get<bool>());

    // Test cellular sending
    EXPECT_EQ(fleet_telemetry_.send_telemetry(json_packet), Status::Ok);
}

// ============================================================================
// 2. OTA Update Manager Tests
// ============================================================================

TEST_F(FleetSubsystemTest, TestOTAPackageUpdateSuccess) {
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0");

    std::string mock_binary = "UADOS_DUMMY_BINARY_DATA_FOR_STANLEY_CONTROLLER_v0.2.0";
    std::string valid_hash = calculate_djb2_hash(mock_binary);

    // Process valid newer update version "0.2.0"
    Status status = ota_manager_.process_ota_update(
        "stanley_steering_hotfix", "0.2.0", valid_hash, mock_binary);

    ASSERT_EQ(status, Status::Ok);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.2.0");
    EXPECT_EQ(ota_manager_.get_rollback_count(), 0);
}

TEST_F(FleetSubsystemTest, TestOTAPackageSemVerRejection) {
    // Process older update version "0.0.5" (active version is stable "0.1.0" -> reject)
    Status status = ota_manager_.process_ota_update(
        "steer_regression_pkg", "0.0.5", "some_hash", "some_binary");

    EXPECT_EQ(status, Status::InvalidArgument);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0"); // stays at active version
}

TEST_F(FleetSubsystemTest, TestOTAPackageChecksumFailureAndRollback) {
    std::string mock_binary = "UADOS_CORRUPTED_PAYLOAD";
    
    // Inject mismatch expected checksum (e.g. "corrupted_expected_hash")
    Status status = ota_manager_.process_ota_update(
        "mpc_control_rollout", "0.3.0", "corrupted_expected_hash", mock_binary);

    EXPECT_EQ(status, Status::InvalidArgument);
    
    // System must reject the package and roll back to active stable "0.1.0" version
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0");
    EXPECT_EQ(ota_manager_.get_rollback_count(), 1);
    EXPECT_EQ(ota_manager_.health(), HealthStatus::Degraded);
}

// ============================================================================
// 3. OTA Manager Boundary & Malformed Inputs Tests (Edge Cases)
// ============================================================================

TEST_F(FleetSubsystemTest, TestOTAManagerMalformedSemVerHandling) {
    // Malformed SemVer strings: system parse_semver returns 0 for non-numeric/empty tokens
    // "abc" parses to 0.0.0, which is not newer than active "0.1.0"
    EXPECT_EQ(ota_manager_.process_ota_update("hotfix", "abc", "hash", "payload"), Status::InvalidArgument);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0");

    // Empty version string -> parses to 0.0.0 -> reject
    EXPECT_EQ(ota_manager_.process_ota_update("hotfix", "", "hash", "payload"), Status::InvalidArgument);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0");

    // "1.2.3.4" (extra parts) -> the current parser parses "1", "2", "3" and ignores "4" or parses it if there is loop,
    // but the loop compares size up to 3: cur_v has size 3 (0,1,0), inc_v parses "1", "2", "3", "4" so size is 4.
    // Loop checks inc_v[i] > cur_v[i] for i in [0..2]. So "1.2.3.4" behaves as "1.2.3".
    // "1.2.3" > "0.1.0", so it should accept if checksum is valid!
    std::string payload = "test_payload";
    std::string valid_hash = calculate_djb2_hash(payload);
    
    // Attempting an invalid checksum on multi-token version "1.2.3.4"
    EXPECT_EQ(ota_manager_.process_ota_update("hotfix", "1.2.3.4", "invalid_hash", payload), Status::InvalidArgument);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0"); // stays at 0.1.0

    // Negative numbers in version (e.g. "-1.-2.-3")
    // std::stoi("-1") works, yielding -1, which is smaller than 0.
    EXPECT_EQ(ota_manager_.process_ota_update("hotfix", "-1.-2.-3", "hash", "payload"), Status::InvalidArgument);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.1.0");
}

TEST_F(FleetSubsystemTest, TestOTAPackageEmptyPayload) {
    // Empty binary payload: verify hash verification works correctly
    std::string empty_payload = "";
    std::string empty_hash = calculate_djb2_hash(empty_payload);

    Status status = ota_manager_.process_ota_update(
        "empty_pkg", "0.2.0", empty_hash, empty_payload);

    ASSERT_EQ(status, Status::Ok);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.2.0");
}

TEST_F(FleetSubsystemTest, TestOTARollbackOverflowAndRecovery) {
    // Inject multiple corrupted rollouts and confirm health and incrementing rollback metrics
    EXPECT_EQ(ota_manager_.get_rollback_count(), 0);
    EXPECT_EQ(ota_manager_.health(), HealthStatus::Healthy);

    for (int i = 1; i <= 5; ++i) {
        Status status = ota_manager_.process_ota_update(
            "bad_pkg_" + std::to_string(i), "0.3.0", "corrupted_hash", "some_payload");
        EXPECT_EQ(status, Status::InvalidArgument);
        EXPECT_EQ(ota_manager_.get_rollback_count(), i);
        EXPECT_EQ(ota_manager_.health(), HealthStatus::Degraded);
    }

    // Recover health state to Healthy upon a valid newer package installation
    std::string payload = "recovered_binary";
    std::string valid_hash = calculate_djb2_hash(payload);
    Status status = ota_manager_.process_ota_update("stable_pkg", "0.4.0", valid_hash, payload);
    
    EXPECT_EQ(status, Status::Ok);
    EXPECT_EQ(ota_manager_.get_active_version(), "0.4.0");
    EXPECT_EQ(ota_manager_.health(), HealthStatus::Healthy); // Restored to Healthy!

    // Resetting metrics
    ota_manager_.reset_metrics();
    EXPECT_EQ(ota_manager_.get_rollback_count(), 0);
}

