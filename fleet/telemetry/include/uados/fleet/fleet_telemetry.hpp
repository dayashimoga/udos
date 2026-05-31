#pragma once

/// @file fleet_telemetry.hpp
/// @brief Fleet telemetry ingestion and cloud packaging.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::fleet {

/// @brief Fleet Telemetry Packaging component.
///
/// Packs local vehicle kinematics, health states, and path tracking errors
/// into structured JSON payloads for cellular transmission to simulated cloud endpoints.
class FleetTelemetry final : public uados::core::ComponentBase {
public:
    FleetTelemetry() = default;
    ~FleetTelemetry() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "fleet.telemetry"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Assembles a cell-compatible JSON payload string
    /// @param state Current vehicle kinematic state
    /// @param cross_track_error Calculated lateral path deviation (m)
    /// @param heading_error Calculated heading path deviation (rad)
    /// @param is_emergency_active True if Safety Monitor emergency override is active
    /// @return Serialized JSON payload string
    [[nodiscard]] std::string package_telemetry(
        const VehicleState& state,
        double cross_track_error,
        double heading_error,
        bool is_emergency_active) const noexcept;

    /// Simulates gRPC/cellular transmission of payload
    /// @param payload Serialized telemetry JSON
    /// @return Status::Ok on successful transmission
    [[nodiscard]] Status send_telemetry(const std::string& payload) const noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    std::string vehicle_uuid_{"uados-ego-carla-001"};
};

} // namespace uados::fleet
