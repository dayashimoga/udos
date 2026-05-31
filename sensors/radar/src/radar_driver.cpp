#include "uados/sensors/radar_driver.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::sensors {

UADOS_DECLARE_LOGGER("sensors.radar")

RadarDriver::~RadarDriver() {
    stop();
}

Status RadarDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Radar Driver...");

    if (config && config["sensor_id"]) {
        sensor_id_ = config["sensor_id"].as<std::string>();
    }

    extrinsics_.mount_pose.position = {2.0, 0.0, 0.5}; // Front bumper
    extrinsics_.mount_pose.orientation = Quat::Identity();

    health_.status = HealthStatus::Healthy;
    health_.data_rate_hz = 20.0;
    health_.expected_rate_hz = 20.0;
    health_.dropped_frames = 0;
    health_.last_data = Clock::now();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Radar Driver '{}' initialized", sensor_id_);
    return Status::Ok;
}

Status RadarDriver::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status RadarDriver::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::shared_ptr<SensorData> RadarDriver::read() {
    std::lock_guard lock(mutex_);
    if (!active_) return nullptr;

    auto scan = std::make_shared<RadarScan>();
    scan->timestamp = Clock::now();
    scan->sequence = seq_++;
    scan->sensor_type = SensorType::Radar;
    scan->sensor_id = sensor_id_;

    // Generate two synthetic tracked targets
    scan->detections.resize(2);

    // Target 1: Vehicle 15m ahead, stationary relative to world (so moving -5m/s relative to us)
    scan->detections[0].range = 15.0f;
    scan->detections[0].azimuth = 0.05f; // slightly off-center
    scan->detections[0].elevation = 0.0f;
    scan->detections[0].velocity = -5.0f;
    scan->detections[0].rcs = 15.0f; // Typical small car RCS
    scan->detections[0].snr = 25.0f;

    // Target 2: Pedestrian 8m ahead, moving across (velocity -0.5m/s relative)
    scan->detections[1].range = 8.0f;
    scan->detections[1].azimuth = -0.15f;
    scan->detections[1].elevation = 0.0f;
    scan->detections[1].velocity = -0.5f;
    scan->detections[1].rcs = 0.0f;  // Typical pedestrian RCS
    scan->detections[1].snr = 12.0f;

    health_.last_data = scan->timestamp;
    health_.total_heartbeats++;

    return scan;
}

Extrinsics RadarDriver::extrinsics() const {
    std::lock_guard lock(mutex_);
    return extrinsics_;
}

void RadarDriver::set_extrinsics(const Extrinsics& ext) {
    std::lock_guard lock(mutex_);
    extrinsics_ = ext;
}

SensorHealth RadarDriver::sensor_health() const {
    std::lock_guard lock(mutex_);
    return health_;
}

bool RadarDriver::is_active() const {
    std::lock_guard lock(mutex_);
    return active_;
}

} // namespace uados::sensors
