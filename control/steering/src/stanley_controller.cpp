#include "uados/control/stanley_controller.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

namespace uados::control {

void StanleyController::configure(double gain_cross_track, double gain_feedforward, double max_steering_angle) noexcept {
    k_e_ = gain_cross_track;
    k_ff_ = gain_feedforward;
    max_steer_ = max_steering_angle;
}

double StanleyController::calculate_steering(
    const VehicleState& state,
    const Trajectory& trajectory,
    double& out_cross_track_error,
    double& out_heading_error) const {

    out_cross_track_error = 0.0;
    out_heading_error = 0.0;

    if (trajectory.points.empty()) {
        return 0.0;
    }

    double x_ego = state.position.x;
    double y_ego = state.position.y;

    // Extract ego yaw heading (rotation around z axle)
    double yaw_ego = state.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    if (std::isnan(yaw_ego)) {
        yaw_ego = 0.0;
    }

    // 1. Project front axle coordinates (Stanley regulates front axle deviation)
    double x_f = x_ego + wheelbase_ * std::cos(yaw_ego);
    double y_f = y_ego + wheelbase_ * std::sin(yaw_ego);

    // 2. Find nearest trajectory waypoint index
    double min_dist_sq = std::numeric_limits<double>::max();
    size_t closest_idx = 0;

    for (size_t i = 0; i < trajectory.points.size(); ++i) {
        const auto& pt = trajectory.points[i];
        double dx = x_f - pt.position.x;
        double dy = y_f - pt.position.y;
        double dist_sq = dx * dx + dy * dy;
        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            closest_idx = i;
        }
    }

    const auto& closest_pt = trajectory.points[closest_idx];

    // 3. Compute cross-track error (e) with directional sign
    double dx = x_f - closest_pt.position.x;
    double dy = y_f - closest_pt.position.y;
    double dist = std::sqrt(dx * dx + dy * dy);

    // Cross product of path heading direction vector and error vector to find sign
    // Path vector: D = (cos(yaw_p), sin(yaw_p))
    // Error vector: E = (dx, dy)
    double yaw_path = closest_pt.heading;
    double cross_product = std::cos(yaw_path) * dy - std::sin(yaw_path) * dx;
    
    // Positive if ego is to the left of the path, negative if to the right
    double sign = (cross_product >= 0.0) ? 1.0 : -1.0;
    double cross_track_error = sign * dist;

    // 4. Compute heading error (theta_e)
    double heading_error = yaw_path - yaw_ego;

    // Normalize heading error to [-pi, pi]
    while (heading_error > M_PI)  heading_error -= 2.0 * M_PI;
    while (heading_error < -M_PI) heading_error += 2.0 * M_PI;

    // 5. Stanley control law computation
    double speed = std::abs(state.velocity.vx); // Use absolute value to support reverse motion safely
    
    double steer_feedback = std::atan2(k_e_ * cross_track_error, speed + epsilon_);
    double steer_feedforward = k_ff_ * closest_pt.curvature;

    double steer_cmd = heading_error + steer_feedback + steer_feedforward;

    // Clamp output steer command between mechanical bounds
    steer_cmd = std::clamp(steer_cmd, -max_steer_, max_steer_);

    // Output variables
    out_cross_track_error = cross_track_error;
    out_heading_error = heading_error;

    return steer_cmd;
}

} // namespace uados::control
