#include "uados/hal/carla_driver.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::hal {

UADOS_DECLARE_LOGGER("hal.driver.carla")

CARLADriver::~CARLADriver() {
    stop();
}

Status CARLADriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing CARLA vehicle driver...");

    // Setup capabilities (standard CARLA reference vehicle - e.g., Tesla Model 3)
    caps_.max_steering_angle = 0.524; // ~30 degrees in radians
    caps_.max_speed = 35.0;           // m/s (~126 km/h)
    caps_.max_acceleration = 4.0;     // m/s^2
    caps_.max_deceleration = 8.0;     // m/s^2
    caps_.wheelbase = 2.875;          // meters
    caps_.track_width = 1.6;          // meters
    caps_.has_steering = true;
    caps_.has_throttle = true;
    caps_.has_brake = true;
    caps_.has_gear = true;

    // Set initial state
    state_.timestamp = Clock::now();
    state_.position = {0.0, 0.0, 0.0};
    state_.velocity = {0.0, 0.0, 0.0};
    state_.acceleration = {0.0, 0.0, 0.0};
    state_.orientation = Quat::Identity();
    state_.steering_angle = 0.0;
    state_.wheel_speeds = {0.0, 0.0, 0.0, 0.0};
    state_.gear = GearPosition::Drive;
    state_.battery_voltage = 13.8;
    state_.engine_running = true;

    last_update_time_ = Clock::now();

    // Set driver status
    status_.health = HealthStatus::Healthy;
    status_.state = ComponentState::Initialized;
    status_.connected = false;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("CARLA vehicle driver initialized successfully");
    return Status::Ok;
}

Status CARLADriver::start() {
    std::lock_guard lock(mutex_);
    
    if (state() == ComponentState::Running) {
        return Status::Ok;
    }

    UADOS_LOG_INFO("Starting CARLA driver connections...");
    connected_ = true;
    status_.connected = true;
    status_.state = ComponentState::Running;
    
    last_update_time_ = Clock::now();
    set_state(ComponentState::Running);

    UADOS_LOG_INFO("CARLA driver started successfully");
    return Status::Ok;
}

Status CARLADriver::stop() {
    std::lock_guard lock(mutex_);
    
    if (state() == ComponentState::Stopped) {
        return Status::Ok;
    }

    UADOS_LOG_INFO("Stopping CARLA driver connections...");
    connected_ = false;
    status_.connected = false;
    status_.state = ComponentState::Stopped;
    set_state(ComponentState::Stopped);

    return Status::Ok;
}

uados::Result<VehicleState> CARLADriver::read_state() {
    std::lock_guard lock(mutex_);
    
    if (!connected_) {
        return uados::Result<VehicleState>::error(Status::NotReady, "Driver is not connected");
    }

    update_simulation();
    status_.states_received++;
    return uados::Result<VehicleState>::success(state_);
}

uados::Status CARLADriver::write_command(const VehicleCommand& cmd) {
    std::lock_guard lock(mutex_);
    
    if (!connected_) {
        return Status::NotReady;
    }

    last_cmd_ = cmd;
    status_.commands_sent++;
    return Status::Ok;
}

VehicleCapabilities CARLADriver::capabilities() const {
    std::lock_guard lock(mutex_);
    return caps_;
}

DriverStatus CARLADriver::driver_status() const {
    std::lock_guard lock(mutex_);
    return status_;
}

bool CARLADriver::is_connected() const {
    std::lock_guard lock(mutex_);
    return connected_;
}

uados::Status CARLADriver::emergency_stop() {
    std::lock_guard lock(mutex_);
    
    UADOS_LOG_WARN("CARLA Driver: EMERGENCY STOP triggered");
    
    last_cmd_.throttle = 0.0;
    last_cmd_.brake = 1.0;
    last_cmd_.emergency_stop = true;
    
    state_.velocity = {0.0, 0.0, 0.0};
    state_.acceleration = {0.0, 0.0, 0.0};
    
    return Status::Ok;
}

void CARLADriver::update_simulation() {
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time_);
    double dt = static_cast<double>(elapsed.count()) / 1e6; // to seconds
    last_update_time_ = now;

    if (dt <= 0.0) return;

    // 1. Simulating Longitudinal Dynamics
    double current_speed = state_.velocity.magnitude();
    double accel = 0.0;

    if (last_cmd_.emergency_stop) {
        accel = -caps_.max_deceleration;
    } else {
        double throttle_force = last_cmd_.throttle * caps_.max_acceleration;
        double braking_force = last_cmd_.brake * caps_.max_deceleration;
        accel = throttle_force - braking_force;

        // Apply simple drag
        accel -= 0.05 * current_speed;
    }

    // Update Speed
    double new_speed = current_speed + accel * dt;
    if (new_speed < 0.0) new_speed = 0.0;
    if (new_speed > caps_.max_speed) new_speed = caps_.max_speed;

    // 2. Simulating Lateral Dynamics (Kinematic Bicycle Model)
    // L is wheelbase
    double L = caps_.wheelbase;
    double delta = last_cmd_.steering_angle; // Front wheel steer angle

    // Slip angle beta at center of gravity
    double beta = std::atan(0.5 * std::tan(delta));

    // Get current yaw from quaternion
    double yaw = state_.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();

    // Equations of motion
    double dx = new_speed * std::cos(yaw + beta);
    double dy = new_speed * std::sin(yaw + beta);
    double dyaw = (new_speed / L) * std::cos(beta) * std::tan(delta);

    // Update Position
    state_.position.x += dx * dt;
    state_.position.y += dy * dt;

    // Update Yaw
    yaw += dyaw * dt;

    // Set updated orientations
    state_.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                         Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                         Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ());

    // Update Velocity and Acceleration State
    state_.velocity.vx = dx;
    state_.velocity.vy = dy;
    state_.acceleration.ax = accel * std::cos(yaw);
    state_.acceleration.ay = accel * std::sin(yaw);

    state_.steering_angle = delta;
    state_.timestamp = now;

    // Update wheel speeds
    double wheel_rot = new_speed / 0.34; // 34cm tire radius
    state_.wheel_speeds = {wheel_rot, wheel_rot, wheel_rot, wheel_rot};
}

} // namespace uados::hal
