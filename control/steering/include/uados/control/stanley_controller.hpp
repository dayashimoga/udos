#pragma once

/// @file stanley_controller.hpp
/// @brief Geometric Stanley steering path tracking controller.

#include "uados/types.hpp"

namespace uados::control {

/// @brief Stanley steering path tracker.
///
/// Implements the front-axle Stanley lateral tracking model:
/// steering = heading_error + arctan(k_e * cross_track_error / (speed + epsilon)) + feedforward
class StanleyController final {
public:
    StanleyController() = default;
    ~StanleyController() = default;

    /// Configures the Stanley steering controller parameters
    /// @param gain_cross_track Position error corrective gain (k_e)
    /// @param gain_feedforward Curvature feedforward gain (k_ff)
    /// @param max_steering_angle Maximum steering angle limit (rad)
    void configure(double gain_cross_track, double gain_feedforward, double max_steering_angle) noexcept;

    /// Computes steering angle command based on vehicle pose and target trajectory
    /// @param state Current vehicle kinematic pose and velocity
    /// @param trajectory Target reference trajectory path
    /// @param out_cross_track_error Output param to write calculated lateral deviation (m)
    /// @param out_heading_error Output param to write calculated heading error (rad)
    /// @return Steering angle command (rad) capped by mechanical constraints
    [[nodiscard]] double calculate_steering(
        const VehicleState& state,
        const Trajectory& trajectory,
        double& out_cross_track_error,
        double& out_heading_error) const;

private:
    double k_e_{1.5};                 ///< Cross-track corrective gain
    double k_ff_{1.0};                ///< Curvature feedforward gain
    double max_steer_{0.70};          ///< Maximum steer limit (rad)
    double epsilon_{0.5};             ///< Softening parameter for division at low speed
    double wheelbase_{2.7};           ///< Vehicle wheelbase (m) for front axle projecting
};

} // namespace uados::control
