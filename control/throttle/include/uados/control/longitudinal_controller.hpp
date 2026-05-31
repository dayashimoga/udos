#pragma once

/// @file longitudinal_controller.hpp
/// @brief Speed tracking PID and feedforward longitudinal controller.

#include "uados/types.hpp"

namespace uados::control {

/// @brief Longitudinal throttle and brake regulator.
///
/// Implements speed tracking PID control with integral anti-windup
/// and splits output commands between throttle and brake channels.
class LongitudinalController final {
public:
    LongitudinalController() = default;
    ~LongitudinalController() = default;

    /// Configures the PID gains and physical system capabilities
    /// @param kp Proportional gain
    /// @param ki Integral gain
    /// @param kd Derivative gain
    /// @param max_accel Max longitudinal acceleration capacity (m/s²)
    /// @param max_decel Max longitudinal deceleration capacity (m/s²)
    void configure(double kp, double ki, double kd, double max_accel, double max_decel) noexcept;

    /// Computes throttle and brake command parameters to track reference speed
    /// @param state Current vehicle state (contains velocity and timestamps)
    /// @param trajectory Target reference trajectory
    /// @param dt Time delta since last control loop execution (s)
    /// @param out_speed_error Output speed tracking error (m/s)
    /// @param out_throttle Output calculated throttle command [0.0, 1.0]
    /// @param out_brake Output calculated brake command [0.0, 1.0]
    void calculate_longitudinal(
        const VehicleState& state,
        const Trajectory& trajectory,
        double dt,
        double& out_speed_error,
        double& out_throttle,
        double& out_brake) noexcept;

    /// Resets the internal integral error accumulation (e.g. at transition states)
    void reset() noexcept;

private:
    double kp_{1.0};                  ///< Proportional gain
    double ki_{0.1};                  ///< Integral gain
    double kd_{0.05};                 ///< Derivative gain
    double max_accel_{3.0};           ///< Max acceleration capacity (m/s²)
    double max_decel_{5.0};           ///< Max deceleration capacity (m/s²)

    double integral_error_{0.0};      ///< Accumulated integral error
    double previous_error_{0.0};      ///< Previous loop tracking error
    bool first_run_{true};            ///< Flag to initialize derivative term
};

} // namespace uados::control
