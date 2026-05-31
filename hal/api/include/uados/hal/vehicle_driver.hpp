#pragma once

/// @file vehicle_driver.hpp
/// @brief Vehicle driver abstraction interface.
///
/// All vehicle platforms (CARLA simulation, RC car, production vehicle)
/// implement this interface, enabling the rest of the autonomy stack
/// to operate identically regardless of the underlying vehicle.

#include "uados/types.hpp"
#include "uados/component.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace uados::hal {

// ============================================================================
// Driver Configuration
// ============================================================================

/// Driver-specific configuration
struct DriverConfig {
    std::string driver_type;          ///< Driver class name
    std::string connection_string;    ///< Connection info (IP, port, serial, etc.)
    uados::core::Config params;      ///< Additional YAML parameters
};

/// Driver operational status
struct DriverStatus {
    uados::HealthStatus health{uados::HealthStatus::Unknown};
    uados::ComponentState state{uados::ComponentState::Loaded};
    bool connected{false};
    uint64_t commands_sent{0};
    uint64_t states_received{0};
    uint64_t errors{0};
    uados::Duration avg_latency{};
    std::string last_error;
};

// ============================================================================
// Vehicle Driver Interface
// ============================================================================

/// @brief Abstract interface for all vehicle drivers.
///
/// Vehicle drivers bridge the UADOS autonomy stack to specific vehicle
/// hardware or simulation platforms. The interface provides:
/// - Vehicle state reading (position, velocity, steering, etc.)
/// - Vehicle command writing (steering, throttle, brake)
/// - Capability discovery
/// - Health monitoring
///
/// Implementations:
/// - CARLADriver: CARLA simulation platform
/// - RCCarDriver: 1/10 scale RC car (Arduino/Teensy PWM)
/// - CANBusDriver: Production vehicles via CAN bus
class IVehicleDriver : public uados::core::ComponentBase {
public:
    ~IVehicleDriver() override = default;

    /// Read the current vehicle state
    /// @return Current state with timestamp
    [[nodiscard]] virtual uados::Result<VehicleState> read_state() = 0;

    /// Send a command to the vehicle
    /// @param cmd Desired steering, throttle, brake values
    /// @return Status::Ok if command was accepted
    [[nodiscard]] virtual uados::Status write_command(const VehicleCommand& cmd) = 0;

    /// Get the vehicle's capabilities
    /// @return Static capability description
    [[nodiscard]] virtual VehicleCapabilities capabilities() const = 0;

    /// Get the driver's operational status
    /// @return Current driver status
    [[nodiscard]] virtual DriverStatus driver_status() const = 0;

    /// Check if the driver is connected to the vehicle/sim
    [[nodiscard]] virtual bool is_connected() const = 0;

    /// Emergency stop — immediately cease all motion
    /// @return Status::Ok if emergency stop was executed
    [[nodiscard]] virtual uados::Status emergency_stop() = 0;
};

} // namespace uados::hal
