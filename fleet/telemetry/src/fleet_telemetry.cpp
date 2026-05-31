#include "uados/fleet/fleet_telemetry.hpp"
#include "uados/logging.hpp"

#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace uados::fleet {

UADOS_DECLARE_LOGGER("fleet.telemetry")

using json = nlohmann::json;

Status FleetTelemetry::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Fleet Telemetry Packager...");

    if (config) {
        if (config["vehicle_uuid"]) {
            vehicle_uuid_ = config["vehicle_uuid"].as<std::string>();
        }
    }

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Fleet Telemetry Packager configured with UUID: '{}'", vehicle_uuid_);
    return Status::Ok;
}

Status FleetTelemetry::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status FleetTelemetry::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::string FleetTelemetry::package_telemetry(
    const VehicleState& state,
    double cross_track_error,
    double heading_error,
    bool is_emergency_active) const noexcept {
    std::lock_guard lock(mutex_);

    if (!active_) {
        return "";
    }

    // Format current system wall time to ISO 8601 string
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&now_c), "%Y-%m-%dT%H:%M:%SZ");
    std::string timestamp_iso = ss.str();

    json packet;
    packet["vehicle_id"] = vehicle_uuid_;
    packet["timestamp"] = timestamp_iso;

    // Pack kinematics parameters
    json kinematics;
    kinematics["x"] = state.position.x;
    kinematics["y"] = state.position.y;
    kinematics["vx"] = state.velocity.vx;
    packet["kinematics"] = kinematics;

    // Pack diagnostics health parameters
    json diagnostics;
    diagnostics["health"] = static_cast<int>(health());
    diagnostics["cross_track_error"] = cross_track_error;
    diagnostics["heading_error"] = heading_error;
    diagnostics["emergency_active"] = is_emergency_active;
    packet["diagnostics"] = diagnostics;

    return packet.dump();
}

Status FleetTelemetry::send_telemetry(const std::string& payload) const noexcept {
    std::lock_guard lock(mutex_);

    if (!active_ || payload.empty()) {
        return Status::NotReady;
    }

    UADOS_LOG_INFO("Cellular Telemetry Packet Sent (gRPC simulated): {} bytes", payload.size());
    UADOS_LOG_DEBUG("Telemetry Payload content: {}", payload);

    return Status::Ok;
}

} // namespace uados::fleet
