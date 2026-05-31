#include "uados/sensors/camera_driver.hpp"
#include "uados/logging.hpp"

namespace uados::sensors {

UADOS_DECLARE_LOGGER("sensors.camera")

CameraDriver::~CameraDriver() {
    stop();
}

Status CameraDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Camera Driver...");

    if (config) {
        if (config["sensor_id"]) {
            sensor_id_ = config["sensor_id"].as<std::string>();
        }
        if (config["width"]) {
            width_ = config["width"].as<uint32_t>();
        }
        if (config["height"]) {
            height_ = config["height"].as<uint32_t>();
        }
    }

    // Set some defaults
    extrinsics_.mount_pose.position = {1.5, 0.0, 1.2}; // 1.5m forward, centered, 1.2m high
    extrinsics_.mount_pose.orientation = Quat::Identity();

    health_.status = HealthStatus::Healthy;
    health_.data_rate_hz = 30.0;
    health_.expected_rate_hz = 30.0;
    health_.dropped_frames = 0;
    health_.last_data = Clock::now();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Camera Driver '{}' initialized: {}x{}", sensor_id_, width_, height_);
    return Status::Ok;
}

Status CameraDriver::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    UADOS_LOG_INFO("Starting Camera Acquisition...");
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status CameraDriver::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    UADOS_LOG_INFO("Stopping Camera Acquisition...");
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::shared_ptr<SensorData> CameraDriver::read() {
    std::lock_guard lock(mutex_);
    if (!active_) return nullptr;

    auto frame = std::make_shared<ImageFrame>();
    frame->timestamp = Clock::now();
    frame->sequence = seq_++;
    frame->sensor_type = SensorType::Camera;
    frame->sensor_id = sensor_id_;

    frame->width = width_;
    frame->height = height_;
    frame->channels = 3;
    frame->bytes_per_pixel = 3;

    // Fill with synthetic pattern (gradient)
    size_t size = frame->size_bytes();
    frame->data.resize(size);
    for (size_t i = 0; i < size; i += 3) {
        frame->data[i] = static_cast<uint8_t>((i / 3) % 256);       // Red
        frame->data[i + 1] = static_cast<uint8_t>((i / 10) % 256);  // Green
        frame->data[i + 2] = 128;                                   // Blue
    }

    health_.last_data = frame->timestamp;
    health_.total_heartbeats++;

    return frame;
}

Extrinsics CameraDriver::extrinsics() const {
    std::lock_guard lock(mutex_);
    return extrinsics_;
}

void CameraDriver::set_extrinsics(const Extrinsics& ext) {
    std::lock_guard lock(mutex_);
    extrinsics_ = ext;
}

SensorHealth CameraDriver::sensor_health() const {
    std::lock_guard lock(mutex_);
    return health_;
}

bool CameraDriver::is_active() const {
    std::lock_guard lock(mutex_);
    return active_;
}

} // namespace uados::sensors
