#pragma once

/// @file control_loop.hpp
/// @brief Orchestrates lateral and longitudinal controllers with tracking monitors.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/control/stanley_controller.hpp"
#include "uados/control/longitudinal_controller.hpp"

#include <mutex>
#include <string>

namespace uados::control {

/// @brief Combined control loop orchestrator component.
///
/// Orchestrates the Stanley lateral steering and speed PID longitudinal throttle/brake
/// commands. Operates at $\ge 100$Hz and enforces strict safety error monitoring.
class ControlLoop final : public uados::core::ComponentBase {
public:
    ControlLoop() = default;
    ~ControlLoop() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "control.loops"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Orchestrates lateral and longitudinal loops to output a unified actuator command
    /// @param state Current vehicle kinematic pose and speed feedback
    /// @param trajectory Active reference path waypoints to track
    /// @param dt Cycle time delta (s)
    /// @return Packed VehicleCommand command signals
    [[nodiscard]] VehicleCommand update(
        const VehicleState& state,
        const Trajectory& trajectory,
        double dt);

    // -- Monitoring / Auditing interface --
    [[nodiscard]] double get_cross_track_error() const noexcept { return cross_track_error_; }
    [[nodiscard]] double get_heading_error() const noexcept { return heading_error_; }
    [[nodiscard]] double get_speed_error() const noexcept { return speed_error_; }

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // Sub-controllers
    StanleyController lateral_controller_;
    LongitudinalController longitudinal_controller_;

    // Telemetry state monitors
    double cross_track_error_{0.0};
    double heading_error_{0.0};
    double speed_error_{0.0};

    // Tracking error safety thresholds
    double max_cross_track_limit_{1.5}; ///< Max allowed path cross-track error (m)
    double max_heading_limit_{0.8};     ///< Max allowed path heading error (rad)
};

} // namespace uados::control
