#include "uados/sensors/imu_driver.hpp"
#include "uados/logging.hpp"

#include <random>

namespace uados::sensors {

UADOS_DECLARE_LOGGER("sensors.imu")

IMUDriver::~IMUDriver() {
    stop();
}

Status IMUDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing IMU Driver...");

    if (config && config["sensor_id"]) {
        sensor_id_ = config["sensor_id"].as<std::string>();
    }

    extrinsics_.mount_pose.position = {0.0, 0.0, 0.2}; // Centered close to CG
    extrinsics_.mount_pose.orientation = Quat::Identity();

    health_.status = HealthStatus::Healthy;
    health_.data_rate_hz = 100.0;
    health_.expected_rate_hz = 100.0;
    health_.dropped_frames = 0;
    health_.last_data = Clock::now();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("IMU Driver '{}' initialized", sensor_id_);
    return Status::Ok;
}

Status IMUDriver::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status IMUDriver::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::shared_ptr<SensorData> IMUDriver::read() {
    std::lock_guard lock(mutex_);
    if (!active_) return nullptr;

    auto reading = std::make_shared<IMUReading>();
    reading->timestamp = Clock::now();
    reading->sequence = seq_++;
    reading->sensor_type = SensorType::IMU;
    reading->sensor_id = sensor_id_;

    // Setup noise generators
    static std::mt19937 gen(42);
    static std::normal_distribution<double> dist(0.0, 0.01); // raw noise

    // 1. Accel: 9.81m/s^2 down in Z-direction, slight noise in other directions
    reading->linear_acceleration.ax = dist(gen);
    reading->linear_acceleration.ay = dist(gen);
    reading->linear_acceleration.az = 9.81 + dist(gen); // local gravity

    // 2. Angular velocity: slight random yaw rate
    reading->angular_velocity.x() = dist(gen) * 0.1;
    reading->angular_velocity.y() = dist(gen) * 0.1;
    reading->angular_velocity.z() = 0.02 + dist(gen) * 0.1; // slow yaw turning

    // 3. Orientation: identity (or standard yaw accumulation)
    reading->orientation = Quat::Identity();
    reading->temperature = 25.4 + dist(gen);

    health_.last_data = reading->timestamp;
    health_.total_heartbeats++;

    return reading;
}

Extrinsics IMUDriver::extrinsics() const {
    std::lock_guard lock(mutex_);
    return extrinsics_;
}

void IMUDriver::set_extrinsics(const Extrinsics& ext) {
    std::lock_guard lock(mutex_);
    extrinsics_ = ext;
}

SensorHealth IMUDriver::sensor_health() const {
    std::lock_guard lock(mutex_);
    return health_;
}

bool IMUDriver::is_active() const {
    std::lock_guard lock(mutex_);
    return active_;
}

} // namespace uados::sensors
