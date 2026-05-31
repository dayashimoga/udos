#include "uados/planning/motion_planner.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::planning {

UADOS_DECLARE_LOGGER("planning.motion")

Status MotionPlanner::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Local Motion Planner...");

    if (config) {
        if (config["max_acceleration"]) {
            max_acceleration_ = config["max_acceleration"].as<double>();
        }
        if (config["max_deceleration"]) {
            max_deceleration_ = config["max_deceleration"].as<double>();
        }
        if (config["emergency_deceleration"]) {
            emergency_deceleration_ = config["emergency_deceleration"].as<double>();
        }
        if (config["lateral_accel_limit"]) {
            lateral_accel_limit_ = config["lateral_accel_limit"].as<double>();
        }
        if (config["safety_collision_buffer"]) {
            safety_collision_buffer_ = config["safety_collision_buffer"].as<double>();
        }
    }

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Local Motion Planner initialized successfully.");
    return Status::Ok;
}

Status MotionPlanner::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status MotionPlanner::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

Trajectory MotionPlanner::plan_trajectory(
    const KinematicState& ego_state,
    const BehaviorDecision& behavior_decision,
    const uados::localization::LaneletInfo& current_lanelet_info,
    const std::vector<DetectedObject>& dynamic_obstacles,
    double horizon_seconds,
    double step_seconds) const {
    std::lock_guard lock(mutex_);

    if (!active_) {
        UADOS_LOG_WARN("MotionPlanner: requested plan while inactive.");
        return {};
    }

    // 1. Check for Immediate Fallback Triggers
    if (behavior_decision.emergency_braking) {
        UADOS_LOG_WARN("Emergency braking behavior requested. Synthesizing fallback stop trajectory.");
        return generate_fallback_trajectory(ego_state, horizon_seconds, step_seconds);
    }

    double x0 = ego_state.pose.position.x;
    double y0 = ego_state.pose.position.y;
    double yaw0 = ego_state.pose.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z(); // extract yaw
    if (std::isnan(yaw0)) {
        yaw0 = 0.0;
    }

    double v0 = ego_state.velocity.vx;
    double a0 = ego_state.acceleration.ax;

    // 2. Lateral Boundary Setup & Quintic Solver
    // centerline is y = centerline_y
    double centerline_y = y0 - current_lanelet_info.distance_to_centerline;
    double y_f = centerline_y;
    double dy_f = 0.0;
    double ddy_f = 0.0;

    double dy0 = ego_state.velocity.vy;
    double ddy0 = ego_state.acceleration.ay;

    // Convergence time horizon (e.g. 2.0 seconds for lateral transition)
    double T = 2.0;

    double c0 = y0;
    double c1 = dy0;
    double c2 = 0.5 * ddy0;

    double c3 = 0.0, c4 = 0.0, c5 = 0.0;
    if (T > 0.1) {
        double Y = y_f - (y0 + dy0 * T + 0.5 * ddy0 * T * T);
        double V = dy_f - (dy0 + ddy0 * T);
        double A = ddy_f - ddy0;

        Eigen::Matrix3d M;
        M << T*T*T,   T*T*T*T,   T*T*T*T*T,
             3.0*T*T, 4.0*T*T*T, 5.0*T*T*T*T,
             6.0*T,   12.0*T*T,  20.0*T*T*T;
        Eigen::Vector3d rhs(Y, V, A);
        Eigen::Vector3d coeffs = M.colPivHouseholderQr().solve(rhs);
        c3 = coeffs(0);
        c4 = coeffs(1);
        c5 = coeffs(2);
    }

    // 3. Longitudinal Profile Integration
    double vt = behavior_decision.target_speed;
    Trajectory traj;
    traj.start_time = ego_state.pose.timestamp;
    traj.is_fallback = false;

    int steps = static_cast<int>(horizon_seconds / step_seconds);
    double s = 0.0;
    double v = v0;
    double a = a0;

    // Evaluate stop or cruise planning acceleration
    bool is_stopping = (behavior_decision.state == BehaviorState::StopAtStopLine ||
                         behavior_decision.state == BehaviorState::YieldObstacle) && 
                        behavior_decision.stop_distance > 0.0;

    double acc_long = 0.0;
    double stop_distance = behavior_decision.stop_distance;
    double deceleration_buffer = 1.0; // Stop buffer

    if (is_stopping) {
        double target_stop_dist = std::max(0.2, stop_distance - deceleration_buffer);
        if (v0 > 0.1) {
            acc_long = - (v0 * v0) / (2.0 * target_stop_dist);
            acc_long = std::clamp(acc_long, -max_deceleration_, -0.1);
        } else {
            acc_long = 0.0;
            v = 0.0;
        }
    } else {
        if (v0 < vt) {
            acc_long = max_acceleration_;
        } else if (v0 > vt) {
            acc_long = -max_deceleration_;
        }
    }

    for (int i = 0; i <= steps; ++i) {
        double t = i * step_seconds;
        TrajectoryPoint pt;
        pt.time_offset = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(t));

        // -- Longitudinal state integration --
        double cur_x = x0;
        double cur_v = v;
        double cur_a = acc_long;

        if (is_stopping) {
            double stop_time = (acc_long < -0.01) ? (-v0 / acc_long) : 0.0;
            if (t <= stop_time) {
                cur_x = x0 + v0 * t + 0.5 * acc_long * t * t;
                cur_v = std::max(0.0, v0 + acc_long * t);
                cur_a = acc_long;
            } else {
                cur_x = x0 + v0 * stop_time + 0.5 * acc_long * stop_time * stop_time;
                cur_v = 0.0;
                cur_a = 0.0;
            }
        } else {
            double trans_time = (std::abs(acc_long) > 0.01) ? ((vt - v0) / acc_long) : 0.0;
            if (trans_time > 0.0 && t <= trans_time) {
                cur_x = x0 + v0 * t + 0.5 * acc_long * t * t;
                cur_v = v0 + acc_long * t;
                cur_a = acc_long;
            } else {
                double x_trans = x0 + v0 * trans_time + 0.5 * acc_long * trans_time * trans_time;
                cur_x = x_trans + vt * (t - trans_time);
                cur_v = vt;
                cur_a = 0.0;
            }
        }

        // -- Lateral state calculation using quintic curve --
        double cur_y = centerline_y;
        double d_y = 0.0;
        double dd_y = 0.0;

        if (t <= T) {
            cur_y = c0 + c1*t + c2*t*t + c3*t*t*t + c4*t*t*t*t + c5*t*t*t*t*t;
            d_y = c1 + 2.0*c2*t + 3.0*c3*t*t + 4.0*c4*t*t*t + 5.0*c5*t*t*t*t;
            dd_y = 2.0*c2 + 6.0*c3*t + 12.0*c4*t*t + 20.0*c5*t*t*t;
        } else {
            // Keep center coordinate
            cur_y = y_f;
            d_y = 0.0;
            dd_y = 0.0;
        }

        // -- Assemble waypoint details --
        pt.position.x = cur_x;
        pt.position.y = cur_y;
        pt.position.z = 0.0;
        pt.speed = cur_v;
        pt.acceleration = cur_a;
        
        // Calculate tangent heading
        double heading = yaw0;
        if (std::abs(cur_v) > 0.1) {
            heading = std::atan2(d_y, cur_v) + yaw0;
        }
        pt.heading = heading;

        // Curvature calculation
        double curvature = 0.0;
        double denom = cur_v * cur_v + d_y * d_y;
        if (denom > 0.01) {
            curvature = (cur_v * dd_y - d_y * cur_a) / (denom * std::sqrt(denom));
        }
        pt.curvature = curvature;

        traj.points.push_back(pt);
    }

    // 4. Proactive Collision Checks
    if (check_collision(traj, dynamic_obstacles)) {
        UADOS_LOG_WARN("Planned trajectory is unsafe (collides with dynamic obstacles). Reverting to fallback safety path.");
        return generate_fallback_trajectory(ego_state, horizon_seconds, step_seconds);
    }

    UADOS_LOG_DEBUG("Local Trajectory generated successfully: {} points", traj.points.size());
    return traj;
}

bool MotionPlanner::check_collision(
    const Trajectory& trajectory,
    const std::vector<DetectedObject>& dynamic_obstacles) const {
    
    for (const auto& pt : trajectory.points) {
        double t_sec = static_cast<double>(pt.time_offset.count()) / 1e9;

        for (const auto& obs : dynamic_obstacles) {
            // Project dynamic obstacle linearly over time offset
            double obs_x = obs.position.x + obs.velocity.vx * t_sec;
            double obs_y = obs.position.y + obs.velocity.vy * t_sec;

            double dx = pt.position.x - obs_x;
            double dy = pt.position.y - obs_y;
            double distance = std::sqrt(dx * dx + dy * dy);

            if (distance < safety_collision_buffer_) {
                UADOS_LOG_WARN("Collision detected at t={:.2f}s with obstacle ID={}: distance={:.2f}m",
                               t_sec, obs.id, distance);
                return true;
            }
        }
    }
    return false;
}

Trajectory MotionPlanner::generate_fallback_trajectory(
    const KinematicState& ego_state,
    double horizon_seconds,
    double step_seconds) const {
    
    double x0 = ego_state.pose.position.x;
    double y0 = ego_state.pose.position.y;
    double yaw0 = ego_state.pose.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    if (std::isnan(yaw0)) {
        yaw0 = 0.0;
    }

    double v0 = ego_state.velocity.vx;

    Trajectory traj;
    traj.start_time = ego_state.pose.timestamp;
    traj.is_fallback = true;

    // Rapid deceleration to stop
    double a_dec = emergency_deceleration_;
    int steps = static_cast<int>(horizon_seconds / step_seconds);

    for (int i = 0; i <= steps; ++i) {
        double t = i * step_seconds;
        TrajectoryPoint pt;
        pt.time_offset = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(t));

        double stop_time = (v0 > 0.1) ? (v0 / a_dec) : 0.0;
        double s = 0.0;
        double v = 0.0;
        double a = 0.0;

        if (t <= stop_time) {
            s = v0 * t - 0.5 * a_dec * t * t;
            v = v0 - a_dec * t;
            a = -a_dec;
        } else {
            s = v0 * stop_time - 0.5 * a_dec * stop_time * stop_time;
            v = 0.0;
            a = 0.0;
        }

        // Project straight along the current heading
        pt.position.x = x0 + s * std::cos(yaw0);
        pt.position.y = y0 + s * std::sin(yaw0);
        pt.position.z = 0.0;
        pt.speed = v;
        pt.acceleration = a;
        pt.heading = yaw0;
        pt.curvature = 0.0;

        traj.points.push_back(pt);
    }

    UADOS_LOG_INFO("Emergency Fallback trajectory synthesized: decelerating at -{:.1f} m/s² to full stop", a_dec);
    return traj;
}

} // namespace uados::planning
