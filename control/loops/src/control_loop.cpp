#include "uados/control/control_loop.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::control {

UADOS_DECLARE_LOGGER("control.loops")

Status ControlLoop::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Control Loop Orchestrator...");

    double kp = 1.2, ki = 0.1, kd = 0.05;
    double max_accel = 3.0, max_decel = 5.0;
    double ke = 1.5, k_ff = 1.0, max_steer = 0.70;

    if (config) {
        // Parse Stanley Gains
        if (config["stanley_gain_cross_track"]) {
            ke = config["stanley_gain_cross_track"].as<double>();
        }
        if (config["stanley_gain_feedforward"]) {
            k_ff = config["stanley_gain_feedforward"].as<double>();
        }
        if (config["max_steering_angle"]) {
            max_steer = config["max_steering_angle"].as<double>();
        }

        // Parse Speed PID Gains
        if (config["speed_kp"]) kp = config["speed_kp"].as<double>();
        if (config["speed_ki"]) ki = config["speed_ki"].as<double>();
        if (config["speed_kd"]) kd = config["speed_kd"].as<double>();
        if (config["max_acceleration"]) max_accel = config["max_acceleration"].as<double>();
        if (config["max_deceleration"]) max_decel = config["max_deceleration"].as<double>();

        // Parse Safety limits
        if (config["max_cross_track_limit"]) {
            max_cross_track_limit_ = config["max_cross_track_limit"].as<double>();
        }
        if (config["max_heading_limit"]) {
            max_heading_limit_ = config["max_heading_limit"].as<double>();
        }
    }

    lateral_controller_.configure(ke, k_ff, max_steer);
    longitudinal_controller_.configure(kp, ki, kd, max_accel, max_decel);

    cross_track_error_ = 0.0;
    heading_error_ = 0.0;
    speed_error_ = 0.0;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Control Loop Orchestrator successfully configured and initialized.");
    return Status::Ok;
}

Status ControlLoop::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    
    longitudinal_controller_.reset();
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status ControlLoop::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

VehicleCommand ControlLoop::update(
    const VehicleState& state,
    const Trajectory& trajectory,
    double dt) {
    std::lock_guard lock(mutex_);

    if (!active_) {
        UADOS_LOG_WARN("ControlLoop: update cycle requested while inactive.");
        return {};
    }

    VehicleCommand command;
    command.timestamp = state.timestamp;
    command.gear = GearPosition::Drive;
    command.emergency_stop = false;

    // 1. Comfortable Safe Stop if path trajectory is empty
    if (trajectory.points.empty()) {
        command.steering_angle = 0.0;
        command.throttle = 0.0;
        command.brake = 1.0; // clamp braking force
        return command;
    }

    // 2. Compute Lateral Control
    double steering = lateral_controller_.calculate_steering(
        state, trajectory, cross_track_error_, heading_error_);

    // 3. Compute Longitudinal Control
    double throttle = 0.0;
    double brake = 0.0;
    longitudinal_controller_.calculate_longitudinal(
        state, trajectory, dt, speed_error_, throttle, brake);

    command.steering_angle = steering;
    command.throttle = throttle;
    command.brake = brake;

    // 4. Force strict safety braking if trajectory indicates safe fallback
    if (trajectory.is_fallback) {
        command.throttle = 0.0;
        command.brake = std::max(command.brake, 0.8);
    }

    // 5. Tracking Error Safety Audits
    bool cross_track_violation = std::abs(cross_track_error_) > max_cross_track_limit_;
    bool heading_violation = std::abs(heading_error_) > max_heading_limit_;

    if (cross_track_violation || heading_violation) {
        UADOS_LOG_WARN("SAFETY VIOLATION: Path tracking error bounds exceeded! Cross-track={:.2f}m (limit={:.1f}m), Heading={:.2f}rad (limit={:.1f}rad)",
                       cross_track_error_, max_cross_track_limit_, heading_error_, max_heading_limit_);
        
        // Safety Envelope emergency override
        command.emergency_stop = true;
        command.throttle = 0.0;
        command.brake = 1.0; // Apply full emergency stop brakes
        set_health(HealthStatus::Degraded);
    } else {
        set_health(HealthStatus::Healthy);
    }

    UADOS_LOG_DEBUG("Control Cycle: CTE={:.2f}m, HE={:.2f}rad, VE={:.2f}m/s -> Steer={:.2f}rad, Throttle={:.2f}, Brake={:.2f}",
                    cross_track_error_, heading_error_, speed_error_, command.steering_angle, command.throttle, command.brake);

    return command;
}

} // namespace uados::control
