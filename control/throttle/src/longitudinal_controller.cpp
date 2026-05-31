#include "uados/control/longitudinal_controller.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

namespace uados::control {

void LongitudinalController::configure(double kp, double ki, double kd, double max_accel, double max_decel) noexcept {
    kp_ = kp;
    ki_ = ki;
    kd_ = kd;
    max_accel_ = max_accel;
    max_decel_ = max_decel;
}

void LongitudinalController::calculate_longitudinal(
    const VehicleState& state,
    const Trajectory& trajectory,
    double dt,
    double& out_speed_error,
    double& out_throttle,
    double& out_brake) noexcept {

    out_speed_error = 0.0;
    out_throttle = 0.0;
    out_brake = 0.0;

    if (trajectory.points.empty()) {
        return;
    }

    double x_ego = state.position.x;
    double y_ego = state.position.y;
    double v_ego = state.velocity.vx;

    // 1. Find the closest trajectory waypoint index
    double min_dist_sq = std::numeric_limits<double>::max();
    size_t closest_idx = 0;

    for (size_t i = 0; i < trajectory.points.size(); ++i) {
        const auto& pt = trajectory.points[i];
        double dx = x_ego - pt.position.x;
        double dy = y_ego - pt.position.y;
        double dist_sq = dx * dx + dy * dy;
        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            closest_idx = i;
        }
    }

    const auto& closest_pt = trajectory.points[closest_idx];
    double v_target = closest_pt.speed;
    double a_feedforward = closest_pt.acceleration;

    // 2. Compute feedback error terms
    double speed_error = v_target - v_ego;
    out_speed_error = speed_error;

    double derivative_error = 0.0;
    if (dt > 0.0001) {
        // Integrate integral with anti-windup clamping limits
        integral_error_ += speed_error * dt;
        double max_i_windup = 5.0; // limit integral contribution to +/- 5 m/s^2
        integral_error_ = std::clamp(integral_error_, -max_i_windup / ki_, max_i_windup / ki_);

        if (!first_run_) {
            derivative_error = (speed_error - previous_error_) / dt;
        } else {
            first_run_ = false;
        }
        previous_error_ = speed_error;
    }

    // 3. Compute acceleration command using PID + Feedforward
    double a_cmd = (kp_ * speed_error) + (ki_ * integral_error_) + (kd_ * derivative_error) + a_feedforward;

    // 4. Split acceleration command into throttle and brake channels
    if (a_cmd >= 0.0) {
        out_throttle = std::clamp(a_cmd / max_accel_, 0.0, 1.0);
        out_brake = 0.0;
    } else {
        out_throttle = 0.0;
        out_brake = std::clamp(-a_cmd / max_decel_, 0.0, 1.0);
    }
}

void LongitudinalController::reset() noexcept {
    integral_error_ = 0.0;
    previous_error_ = 0.0;
    first_run_ = true;
}

} // namespace uados::control
