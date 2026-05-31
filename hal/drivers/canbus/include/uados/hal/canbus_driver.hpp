#pragma once

/// @file canbus_driver.hpp
/// @brief CAN Bus Drive-By-Wire (DBW) vehicle reference driver.

#include "uados/hal/vehicle_driver.hpp"
#include <mutex>
#include <string>

namespace uados::hal {

/// @brief Struct representing a raw CAN frame.
struct CanFrame {
    uint32_t id{0};
    uint8_t dlc{8};
    uint8_t data[8]{0};
};

/// @brief Driver for standard automotive CAN Bus interfaces using SocketCAN or mock channels.
///
/// Encodes high-level commands (steering, throttle, brake) into packed CAN frames,
/// and decodes incoming CAN frames containing vehicle telemetry feedback.
class CANBusDriver final : public IVehicleDriver {
public:
    CANBusDriver() = default;
    ~CANBusDriver() override;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "hal.driver.canbus"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- IVehicleDriver --
    [[nodiscard]] uados::Result<VehicleState> read_state() override;
    [[nodiscard]] uados::Status write_command(const VehicleCommand& cmd) override;
    [[nodiscard]] VehicleCapabilities capabilities() const override;
    [[nodiscard]] DriverStatus driver_status() const override;
    [[nodiscard]] bool is_connected() const override;
    [[nodiscard]] uados::Status emergency_stop() override;

    // -- CAN Debugging Helpers --
    [[nodiscard]] CanFrame encode_command_frame() const noexcept;
    void decode_feedback_frame(const CanFrame& frame) noexcept;

private:
    mutable std::mutex mutex_;
    bool connected_{false};

    VehicleState state_;
    VehicleCommand last_cmd_;
    VehicleCapabilities caps_;
    DriverStatus status_;

    Timestamp last_update_time_;

    void simulate_can_traffic();
};

} // namespace uados::hal
