#include "uados/safety/safety_monitor.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::safety {

UADOS_DECLARE_LOGGER("safety.monitors")

Status SafetyMonitor::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Safety Monitor Engine...");

    if (config) {
        if (config["speed_buffer_mps"]) {
            speed_buffer_mps_ = config["speed_buffer_mps"].as<double>();
        }
        if (config["max_lateral_error_limit"]) {
            max_lateral_error_limit_ = config["max_lateral_error_limit"].as<double>();
        }
        if (config["max_heading_error_limit"]) {
            max_heading_error_limit_ = config["max_heading_error_limit"].as<double>();
        }
        if (config["max_steering_limit"]) {
            max_steering_limit_ = config["max_steering_limit"].as<double>();
        }
    }

    violations_.clear();
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Safety Monitor Engine initialized.");
    return Status::Ok;
}

Status SafetyMonitor::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status SafetyMonitor::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

Status SafetyMonitor::audit_safety(
    const VehicleState& state,
    const uados::localization::LaneletInfo& current_lanelet_info,
    double lateral_error,
    double heading_error,
    VehicleCommand& command) {
    std::lock_guard lock(mutex_);

    if (!active_) {
        return Status::NotReady;
    }

    Status audit_status = Status::Ok;

    // 1. Brake Override System (BOS) Interlock
    if (command.throttle > 0.1 && command.brake > 0.1) {
        record_violation(
            "Brake Override System Interlock",
            SafetySeverity::Warning,
            "Simultaneous throttle (" + std::to_string(command.throttle) + 
            ") and brake (" + std::to_string(command.brake) + ") command requested. Clamping throttle to 0.0."
        );
        command.throttle = 0.0;
    }

    // 2. Mechanical Steering Saturation Guard
    if (std::abs(command.steering_angle) > max_steering_limit_) {
        record_violation(
            "Mechanical Steering Saturation Guard",
            SafetySeverity::Warning,
            "Requested steer angle (" + std::to_string(command.steering_angle) + 
            " rad) exceeds mechanical limit. Clamping."
        );
        command.steering_angle = std::clamp(command.steering_angle, -max_steering_limit_, max_steering_limit_);
    }

    // 3. Speed Limit Boundary
    double v_limit = current_lanelet_info.speed_limit_mps;
    double speed = state.velocity.vx;
    if (speed > v_limit + speed_buffer_mps_) {
        record_violation(
            "Speed Limit Boundary",
            SafetySeverity::Warning,
            "Ego speed (" + std::to_string(speed) + " m/s) exceeds lanelet speed limit (" + 
            std::to_string(v_limit) + " m/s) by margin buffer."
        );
    }

    // 4. ODD Cross-Track Tracking Boundary Breach (Critical)
    bool lateral_breach = std::abs(lateral_error) > max_lateral_error_limit_;
    bool heading_breach = std::abs(heading_error) > max_heading_error_limit_;

    if (lateral_breach || heading_breach || command.emergency_stop) {
        std::string desc = "Critical ODD tracking boundary breach! ";
        if (lateral_breach) {
            desc += "Lateral error (" + std::to_string(lateral_error) + "m) exceeds limit (" + std::to_string(max_lateral_error_limit_) + "m). ";
        }
        if (heading_breach) {
            desc += "Heading error (" + std::to_string(heading_error) + "rad) exceeds limit (" + std::to_string(max_heading_error_limit_) + "rad). ";
        }

        record_violation("ODD Tracking Boundary Breach", SafetySeverity::Emergency, desc);

        // Safety Envelope Override
        command.emergency_stop = true;
        command.throttle = 0.0;
        command.brake = 1.0; // full clamp

        set_health(HealthStatus::Unhealthy);
        audit_status = Status::Error;
    }

    return audit_status;
}

std::vector<SafetyViolation> SafetyMonitor::get_violations() const noexcept {
    std::lock_guard lock(mutex_);
    return violations_;
}

void SafetyMonitor::record_violation(const std::string& rule, SafetySeverity severity, const std::string& desc) noexcept {
    SafetyViolation violation;
    violation.rule_name = rule;
    violation.severity = severity;
    violation.description = desc;
    violation.timestamp = Clock::now();

    violations_.push_back(violation);

    if (severity == SafetySeverity::Emergency) {
        UADOS_LOG_ERROR("SAFETY CRITICAL EMERGENCY: [{}] {}", rule, desc);
    } else {
        UADOS_LOG_WARN("SAFETY WARNING: [{}] {}", rule, desc);
    }
}

} // namespace uados::safety
