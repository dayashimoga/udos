#pragma once

/// @file rc_car_driver.hpp
/// @brief 1/10 Scale RC Car vehicle reference driver.

#include "uados/hal/vehicle_driver.hpp"
#include <mutex>
#include <string>

namespace uados::hal {

/// @brief Driver for a 1/10 scale RC car using serial PWM servo signaling.
///
/// Translates throttle/brake/steering commands into raw PWM microsecond values
/// (typically 1000us to 2000us) sent over serial to an onboard MCU (Teensy/Arduino).
/// Models Ackermann dynamics for simulation testing.
class RCCarDriver final : public IVehicleDriver {
public:
    RCCarDriver() = default;
    ~RCCarDriver() override;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "hal.driver.rc_car"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- IVehicleDriver --
    [[nodiscard]] uados::Result<VehicleState> read_state() override;
    [[nodiscard]] uados::Status write_command(const VehicleCommand& cmd) override;
    [[nodiscard]] VehicleCapabilities capabilities() const override;
    [[nodiscard]] DriverStatus driver_status() const override;
    [[nodiscard]] bool is_connected() const override;
    [[nodiscard]] uados::Status emergency_stop() override;

    // -- RC Car specific helpers --
    [[nodiscard]] uint16_t get_steering_pwm() const noexcept { return steering_pwm_; }
    [[nodiscard]] uint16_t get_throttle_pwm() const noexcept { return throttle_pwm_; }

private:
    mutable std::mutex mutex_;
    bool connected_{false};

    // PWM microsecond values computed
    uint16_t steering_pwm_{1500}; // 1500us neutral
    uint16_t throttle_pwm_{1500}; // 1500us neutral

    VehicleState state_;
    VehicleCommand last_cmd_;
    VehicleCapabilities caps_;
    DriverStatus status_;

    Timestamp last_update_time_;

    void update_pwm_channels(const VehicleCommand& cmd) noexcept;
    void update_simulation();
};

} // namespace uados::hal
