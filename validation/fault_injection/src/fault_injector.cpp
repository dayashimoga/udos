#include "uados/validation/fault_injector.hpp"
#include "uados/logging.hpp"

namespace uados::validation {

UADOS_DECLARE_LOGGER("validation.fault_injection")

Status FaultInjector::init(const uados::core::Config& /*config*/) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Fault Injection and Chaos Engine...");

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Fault Injection and Chaos Engine initialized successfully.");
    return Status::Ok;
}

Status FaultInjector::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status FaultInjector::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void FaultInjector::inject_speed_spike(VehicleState& state, double speed_spike_mps) const noexcept {
    std::lock_guard lock(mutex_);
    if (!active_) return;

    UADOS_LOG_WARN("FAULT INJECTION: Speed spike anomaly (+{:.2f} m/s) injected into vehicle state telemetry.", speed_spike_mps);
    state.velocity.vx += speed_spike_mps;
}

void FaultInjector::inject_lateral_drift(VehicleState& state, double lateral_drift_m) const noexcept {
    std::lock_guard lock(mutex_);
    if (!active_) return;

    UADOS_LOG_WARN("FAULT INJECTION: Lateral offset drift anomaly ({:+.2f} m) injected into localization pose.", lateral_drift_m);
    state.position.y += lateral_drift_m;
}

void FaultInjector::inject_bos_fault(VehicleCommand& command) const noexcept {
    std::lock_guard lock(mutex_);
    if (!active_) return;

    UADOS_LOG_WARN("FAULT INJECTION: Simultaneous throttle (1.0) and brake (1.0) conflict injected into actuator command.");
    command.throttle = 1.0;
    command.brake = 1.0;
}

} // namespace uados::validation
