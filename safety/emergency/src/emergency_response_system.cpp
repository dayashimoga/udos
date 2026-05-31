#include "uados/safety/emergency_response_system.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::safety {

UADOS_DECLARE_LOGGER("safety.emergency")

Status EmergencyResponseSystem::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Emergency Response System (ERS)...");

    if (config) {
        if (config["mrc_deceleration"]) {
            mrc_deceleration_ = config["mrc_deceleration"].as<double>();
        }
    }

    state_ = EmergencyState::Normal;
    time_in_mrc_ = 0.0;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Emergency Response System initialized successfully. MRC deceleration: {:.2f} m/s²", mrc_deceleration_);
    return Status::Ok;
}

Status EmergencyResponseSystem::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    
    active_ = true;
    state_ = EmergencyState::Normal;
    time_in_mrc_ = 0.0;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status EmergencyResponseSystem::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void EmergencyResponseSystem::trigger_mrc() noexcept {
    std::lock_guard lock(mutex_);
    if (state_ == EmergencyState::Normal) {
        UADOS_LOG_WARN("Emergency FSM Transition: Normal -> ActiveMRC (Minimum Risk Condition Triggered!)");
        state_ = EmergencyState::ActiveMRC;
        time_in_mrc_ = 0.0;
    }
}

void EmergencyResponseSystem::reset_nominal() noexcept {
    std::lock_guard lock(mutex_);
    if (state_ != EmergencyState::Normal) {
        UADOS_LOG_INFO("Emergency FSM Reset: {} -> Normal (Nominal status restored)", emergency_state_to_string(state_));
        state_ = EmergencyState::Normal;
        time_in_mrc_ = 0.0;
    }
}

void EmergencyResponseSystem::execute_mrc(const VehicleState& state, VehicleCommand& command, double dt) noexcept {
    std::lock_guard lock(mutex_);

    if (!active_ || state_ == EmergencyState::Normal) {
        return;
    }

    // Force commands to stop vehicle
    command.throttle = 0.0;

    if (state_ == EmergencyState::ActiveMRC) {
        time_in_mrc_ += dt;
        
        // Decelerate smoothly at mrc_deceleration_ (comfort deceleration)
        command.steering_angle = 0.0; // target straight safe pull-over
        command.brake = 0.6;          // map to safe decelerating friction torque

        // Check if vehicle has achieved a complete stop
        if (state.velocity.vx <= 0.1) {
            UADOS_LOG_INFO("Emergency FSM Transition: ActiveMRC -> SafeState (Vehicle has halted, locking Park gear)");
            state_ = EmergencyState::SafeState;
        }
    } else if (state_ == EmergencyState::SafeState) {
        // Enforce absolute Safe State rules
        command.steering_angle = 0.0;
        command.brake = 1.0;                  // full parking brake lock
        command.gear = GearPosition::Park;   // lock transmission
        command.emergency_stop = true;        // trigger warning hazards
    }
}

} // namespace uados::safety
