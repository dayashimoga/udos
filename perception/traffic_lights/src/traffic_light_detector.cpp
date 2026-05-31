#include "uados/perception/traffic_light_detector.hpp"
#include "uados/logging.hpp"

namespace uados::perception {

UADOS_DECLARE_LOGGER("perception.traffic_lights")

Status TrafficLightDetector::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);
    UADOS_LOG_INFO("Initializing Traffic Light Detector...");
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);
    return Status::Ok;
}

Status TrafficLightDetector::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status TrafficLightDetector::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<TrafficLightResult> TrafficLightDetector::detect(const sensors::ImageFrame& frame) {
    std::lock_guard lock(mutex_);
    if (!active_) return {};

    seq_++;
    
    // Cycle state dynamically based on frame sequence:
    // frames 0-100: Green (60% time)
    // frames 101-120: Yellow (15% time)
    // frames 121-180: Red (25% time)
    TrafficLightState simulated_state = TrafficLightState::Green;
    uint64_t phase = seq_ % 180;
    if (phase > 120) {
        simulated_state = TrafficLightState::Red;
    } else if (phase > 100) {
        simulated_state = TrafficLightState::Yellow;
    }

    TrafficLightResult light;
    light.id = 5001; // intersection traffic light ID
    light.state = simulated_state;
    light.confidence = 0.99;
    
    // Positioned 12m ahead, 2m right, 4.5m high (overhead gantry)
    light.position.x = 12.0;
    light.position.y = -2.0;
    light.position.z = 4.5;

    UADOS_LOG_DEBUG("Traffic Light detected: id={}, state={}, pos=({:.1f}, {:.1f}, {:.1f})",
                    light.id,
                    simulated_state == TrafficLightState::Green ? "Green" :
                    (simulated_state == TrafficLightState::Yellow ? "Yellow" : "Red"),
                    light.position.x, light.position.y, light.position.z);

    return {light};
}

} // namespace uados::perception
