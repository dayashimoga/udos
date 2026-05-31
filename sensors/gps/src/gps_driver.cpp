#include "uados/sensors/gps_driver.hpp"
#include "uados/logging.hpp"

#include <random>

namespace uados::sensors {

UADOS_DECLARE_LOGGER("sensors.gps")

GPSDriver::~GPSDriver() {
    stop();
}

Status GPSDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing GPS Driver...");

    if (config) {
        if (config["sensor_id"]) {
            sensor_id_ = config["sensor_id"].as<std::string>();
        }
        if (config["latitude"]) {
            lat_ = config["latitude"].as<double>();
        }
        if (config["longitude"]) {
            lon_ = config["longitude"].as<double>();
        }
        if (config["altitude"]) {
            alt_ = config["altitude"].as<double>();
        }
    }

    extrinsics_.mount_pose.position = {0.0, 0.0, 1.8}; // Roof-mounted GNSS receiver
    extrinsics_.mount_pose.orientation = Quat::Identity();

    health_.status = HealthStatus::Healthy;
    health_.data_rate_hz = 10.0;
    health_.expected_rate_hz = 10.0;
    health_.dropped_frames = 0;
    health_.last_data = Clock::now();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("GPS Driver '{}' initialized at: {:.6f}, {:.6f}", sensor_id_, lat_, lon_);
    return Status::Ok;
}

Status GPSDriver::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status GPSDriver::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::shared_ptr<SensorData> GPSDriver::read() {
    std::lock_guard lock(mutex_);
    if (!active_) return nullptr;

    auto fix = std::make_shared<GPSFix>();
    fix->timestamp = Clock::now();
    fix->sequence = seq_++;
    fix->sensor_type = SensorType::GPS;
    fix->sensor_id = sensor_id_;

    // Add tiny simulated random walk/noise
    static std::mt19937 gen(1337);
    static std::normal_distribution<double> dist(0.0, 0.000002); // ~20cm noise
    
    lat_ += dist(gen);
    lon_ += dist(gen);

    fix->coordinate.latitude = lat_;
    fix->coordinate.longitude = lon_;
    fix->coordinate.altitude = alt_ + dist(gen) * 5.0;

    fix->horizontal_accuracy = 0.15; // 15cm RTK precision
    fix->vertical_accuracy = 0.30;
    fix->speed = 5.0; // 5 m/s motion
    fix->heading = 1.2; // heading angle in radians
    fix->satellites = 18;
    fix->fix_type = GPSFix::FixType::RTK_Fixed;

    health_.last_data = fix->timestamp;
    health_.total_heartbeats++;

    return fix;
}

Extrinsics GPSDriver::extrinsics() const {
    std::lock_guard lock(mutex_);
    return extrinsics_;
}

void GPSDriver::set_extrinsics(const Extrinsics& ext) {
    std::lock_guard lock(mutex_);
    extrinsics_ = ext;
}

SensorHealth GPSDriver::sensor_health() const {
    std::lock_guard lock(mutex_);
    return health_;
}

bool GPSDriver::is_active() const {
    std::lock_guard lock(mutex_);
    return active_;
}

} // namespace uados::sensors
