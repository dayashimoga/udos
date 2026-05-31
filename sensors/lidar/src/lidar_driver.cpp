#include "uados/sensors/lidar_driver.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::sensors {

UADOS_DECLARE_LOGGER("sensors.lidar")

LiDARDriver::~LiDARDriver() {
    stop();
}

Status LiDARDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing LiDAR Driver...");

    if (config) {
        if (config["sensor_id"]) {
            sensor_id_ = config["sensor_id"].as<std::string>();
        }
        if (config["channels"]) {
            channels_ = config["channels"].as<uint32_t>();
        }
        if (config["points_per_channel"]) {
            points_per_channel_ = config["points_per_channel"].as<uint32_t>();
        }
    }

    extrinsics_.mount_pose.position = {0.0, 0.0, 2.0}; // Roof-mounted
    extrinsics_.mount_pose.orientation = Quat::Identity();

    health_.status = HealthStatus::Healthy;
    health_.data_rate_hz = 10.0;
    health_.expected_rate_hz = 10.0;
    health_.dropped_frames = 0;
    health_.last_data = Clock::now();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("LiDAR Driver '{}' initialized: {} channels", sensor_id_, channels_);
    return Status::Ok;
}

Status LiDARDriver::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status LiDARDriver::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::shared_ptr<SensorData> LiDARDriver::read() {
    std::lock_guard lock(mutex_);
    if (!active_) return nullptr;

    auto cloud = std::make_shared<PointCloud>();
    cloud->timestamp = Clock::now();
    cloud->sequence = seq_++;
    cloud->sensor_type = SensorType::LiDAR;
    cloud->sensor_id = sensor_id_;

    size_t total_points = channels_ * points_per_channel_;
    cloud->points.resize(total_points);

    // Generate radial wall patterns
    size_t idx = 0;
    for (uint32_t c = 0; c < channels_; ++c) {
        // Vertical angle (pitch)
        float elevation = -0.26f + (static_cast<float>(c) * 0.52f / static_cast<float>(channels_)); // -15 to +15 deg
        float cos_el = std::cos(elevation);
        float sin_el = std::sin(elevation);

        for (uint32_t p = 0; p < points_per_channel_; ++p) {
            // Horizontal angle (yaw)
            float azimuth = static_cast<float>(p) * 2.0f * 3.14159f / static_cast<float>(points_per_channel_);
            
            // Draw a virtual square room (walls at 10m)
            float range = 10.0f / std::max(std::abs(std::cos(azimuth)), std::abs(std::sin(azimuth)));
            if (range > 30.0f) range = 30.0f; // maximum limit

            LiDARPoint pt;
            pt.x = range * cos_el * std::cos(azimuth);
            pt.y = range * cos_el * std::sin(azimuth);
            pt.z = range * sin_el;
            pt.intensity = 1.0f / (1.0f + 0.05f * range * range); // intensity drops with distance
            pt.ring = static_cast<uint16_t>(c);
            pt.timestamp_offset = 0.0f;

            cloud->points[idx++] = pt;
        }
    }

    cloud->width = points_per_channel_;
    cloud->height = channels_;

    health_.last_data = cloud->timestamp;
    health_.total_heartbeats++;

    return cloud;
}

Extrinsics LiDARDriver::extrinsics() const {
    std::lock_guard lock(mutex_);
    return extrinsics_;
}

void LiDARDriver::set_extrinsics(const Extrinsics& ext) {
    std::lock_guard lock(mutex_);
    extrinsics_ = ext;
}

SensorHealth LiDARDriver::sensor_health() const {
    std::lock_guard lock(mutex_);
    return health_;
}

bool LiDARDriver::is_active() const {
    std::lock_guard lock(mutex_);
    return active_;
}

} // namespace uados::sensors
