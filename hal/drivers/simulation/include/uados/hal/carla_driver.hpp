#pragma once

/// @file carla_driver.hpp
/// @brief CARLA Simulator vehicle reference driver.

#include "uados/hal/vehicle_driver.hpp"
#include <mutex>
#include <string>

namespace uados::hal {

/// @brief Driver for interacting with the CARLA simulator virtual vehicle.
///
/// Under hardware-in-the-loop or simulation configurations, bridges control
/// commands to the CARLA client and queries vehicle telemetry.
/// In virtual/test modes, integrates a high-fidelity kinematic bicycle model.
class CARLADriver final : public IVehicleDriver {
public:
    CARLADriver() = default;
    ~CARLADriver() override;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "hal.driver.carla"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- IVehicleDriver --
    [[nodiscard]] uados::Result<VehicleState> read_state() override;
    [[nodiscard]] uados::Status write_command(const VehicleCommand& cmd) override;
    [[nodiscard]] VehicleCapabilities capabilities() const override;
    [[nodiscard]] DriverStatus driver_status() const override;
    [[nodiscard]] bool is_connected() const override;
    [[nodiscard]] uados::Status emergency_stop() override;

private:
    mutable std::mutex mutex_;
    bool connected_{false};
    
    // Internal Kinematic bicycle model state (for simulation loop)
    VehicleState state_;
    VehicleCommand last_cmd_;
    VehicleCapabilities caps_;
    DriverStatus status_;
    
    Timestamp last_update_time_;
    
    // Kinematic bicycle update step (simulates vehicle moving)
    void update_simulation();
};

} // namespace uados::hal
