#include "uados/hal/safety_envelope.hpp"
#include "uados/logging.hpp"

#include <algorithm>
#include <cmath>

namespace uados::hal {

UADOS_DECLARE_LOGGER("hal.safety_envelope")

SafetyEnvelope::SafetyEnvelope(const VehicleCapabilities& caps)
    : caps_(caps) {
    UADOS_LOG_INFO("SafetyEnvelope initialized: max_steering_angle={:.3f} rad, max_speed={:.1f} m/s",
                   caps_.max_steering_angle, caps_.max_speed);
}

void update_capabilities(const VehicleCapabilities& caps);

void SafetyEnvelope::update_capabilities(const VehicleCapabilities& caps) {
    caps_ = caps;
    UADOS_LOG_INFO("SafetyEnvelope capabilities updated: max_steering_angle={:.3f} rad, max_speed={:.1f} m/s",
                   caps_.max_steering_angle, caps_.max_speed);
}

Status SafetyEnvelope::check_interlocks(const VehicleCommand& cmd) const noexcept {
    // Interlock 1: Throttle and Brake cannot both be heavily applied simultaneously
    // If throttle > 0.05 and brake > 0.05, it violates standard efficiency/safety rules
    // (though we will override and prioritize brake in validation rather than failing)
    if (cmd.throttle > 0.05 && cmd.brake > 0.05) {
        return Status::NotReady; // Signal that override is required
    }
    return Status::Ok;
}

Scalar SafetyEnvelope::calculate_max_steering_angle(Scalar current_speed) const noexcept {
    if (current_speed <= 2.0) {
        return caps_.max_steering_angle; // Full lock allowed at low speed
    }

    // Dynamic steering limit to prevent roll-over at high speeds
    // Limit steering angle inversely proportional to the square of velocity
    // Formula: max_allowed = max_steering_angle / (1.0 + k * v^2)
    // For a car with max_speed = 30m/s and max_steer = 0.5 rad, let's select k = 0.01
    constexpr Scalar k = 0.01;
    Scalar speed_sq = current_speed * current_speed;
    Scalar dynamic_limit = caps_.max_steering_angle / (1.0 + k * speed_sq);

    // Keep within a safe minimum steering block (e.g. 0.05 rad) so we don't completely lock steering at extremely high speeds
    return std::max(dynamic_limit, static_cast<Scalar>(0.05));
}

Result<VehicleCommand> SafetyEnvelope::validate(
    const VehicleCommand& cmd,
    const VehicleState& state) const {

    VehicleCommand safe_cmd = cmd;
    safe_cmd.timestamp = Clock::now();

    // 1. Emergency Stop Handling (Highest Priority Override)
    if (cmd.emergency_stop) {
        safe_cmd.throttle = 0.0;
        safe_cmd.brake = 1.0; // Apply full brakes
        safe_cmd.steering_angle = 0.0; // Center steering
        UADOS_LOG_WARN("Safety Envelope: Emergency Stop active! Overriding all commands.");
        return Result<VehicleCommand>::success(safe_cmd);
    }

    // 2. Actuator Clamping
    safe_cmd.throttle = std::clamp(cmd.throttle, 0.0, 1.0);
    safe_cmd.brake = std::clamp(cmd.brake, 0.0, 1.0);

    // 3. Simultaneous Throttle & Brake Override (Brake Override System)
    if (safe_cmd.throttle > 0.0 && safe_cmd.brake > 0.1) {
        UADOS_LOG_WARN("Safety Envelope: Simultaneous throttle ({:.2f}) and brake ({:.2f}) detected. Overriding throttle to 0.0 (Brake Override System).",
                       safe_cmd.throttle, safe_cmd.brake);
        safe_cmd.throttle = 0.0;
    }

    // 4. Dynamic Steering Limits (Rollover Prevention)
    Scalar current_speed = state.velocity.magnitude();
    Scalar max_steer = calculate_max_steering_angle(current_speed);
    
    if (std::abs(cmd.steering_angle) > max_steer) {
        safe_cmd.steering_angle = std::copysign(max_steer, cmd.steering_angle);
        UADOS_LOG_WARN("Safety Envelope: Steering command {:.3f} rad exceeds dynamic speed limit of {:.3f} rad (speed={:.2f} m/s). Clamping steering.",
                       cmd.steering_angle, max_steer, current_speed);
    } else {
        // Clamp to absolute mechanical limits
        safe_cmd.steering_angle = std::clamp(cmd.steering_angle, -caps_.max_steering_angle, caps_.max_steering_angle);
    }

    return Result<VehicleCommand>::success(safe_cmd);
}

} // namespace uados::hal
