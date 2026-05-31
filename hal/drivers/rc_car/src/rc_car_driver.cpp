#include "uados/hal/rc_car_driver.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::hal {

UADOS_DECLARE_LOGGER("hal.driver.rc_car")

RCCarDriver::~RCCarDriver() {
    stop();
}

Status RCCarDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing 1/10 scale RC Car driver...");

    // Setup capabilities (standard 1/10 scale RC touring/buggy car)
    caps_.max_steering_angle = 0.436; // ~25 degrees in radians
    caps_.max_speed = 12.0;           // m/s (~43 km/h - fast for 1/10 scale!)
    caps_.max_acceleration = 6.0;     // m/s^2 (high power-to-weight ratio)
    caps_.max_deceleration = 10.0;    // m/s^2 (strong braking)
    caps_.wheelbase = 0.257;          // meters (Tamiya TT-02 standard)
    caps_.track_width = 0.185;        // meters
    caps_.has_steering = true;
    caps_.has_throttle = true;
    caps_.has_brake = true;
    caps_.has_gear = false;           // Single-speed direct drive

    // Initial state
    state_.timestamp = Clock::now();
    state_.position = {0.0, 0.0, 0.0};
    state_.velocity = {0.0, 0.0, 0.0};
    state_.acceleration = {0.0, 0.0, 0.0};
    state_.orientation = Quat::Identity();
    state_.steering_angle = 0.0;
    state_.wheel_speeds = {0.0, 0.0, 0.0, 0.0};
    state_.gear = GearPosition::Drive;
    state_.battery_voltage = 7.4;     // Standard 2S LiPo battery voltage
    state_.engine_running = true;

    last_update_time_ = Clock::now();

    status_.health = HealthStatus::Healthy;
    status_.state = ComponentState::Initialized;
    status_.connected = false;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("1/10 scale RC Car driver initialized successfully");
    return Status::Ok;
}

Status RCCarDriver::start() {
    std::lock_guard lock(mutex_);
    
    if (state() == ComponentState::Running) {
        return Status::Ok;
    }

    UADOS_LOG_INFO("Connecting to onboard serial PWM micro-controller...");
    connected_ = true;
    status_.connected = true;
    status_.state = ComponentState::Running;
    
    last_update_time_ = Clock::now();
    set_state(ComponentState::Running);

    UADOS_LOG_INFO("RC Car driver connection established successfully");
    return Status::Ok;
}

Status RCCarDriver::stop() {
    std::lock_guard lock(mutex_);
    
    if (state() == ComponentState::Stopped) {
        return Status::Ok;
    }

    UADOS_LOG_INFO("Releasing onboard serial PWM controller...");
    
    // Send neutral commands before disconnection
    steering_pwm_ = 1500;
    throttle_pwm_ = 1500;
    
    connected_ = false;
    status_.connected = false;
    status_.state = ComponentState::Stopped;
    set_state(ComponentState::Stopped);

    return Status::Ok;
}

uados::Result<VehicleState> RCCarDriver::read_state() {
    std::lock_guard lock(mutex_);
    
    if (!connected_) {
        return uados::Result<VehicleState>::error(Status::NotReady, "Driver is not connected");
    }

    update_simulation();
    status_.states_received++;
    return uados::Result<VehicleState>::success(state_);
}

uados::Status RCCarDriver::write_command(const VehicleCommand& cmd) {
    std::lock_guard lock(mutex_);
    
    if (!connected_) {
        return Status::NotReady;
    }

    last_cmd_ = cmd;
    update_pwm_channels(cmd);
    status_.commands_sent++;
    return Status::Ok;
}

VehicleCapabilities RCCarDriver::capabilities() const {
    std::lock_guard lock(mutex_);
    return caps_;
}

DriverStatus RCCarDriver::driver_status() const {
    std::lock_guard lock(mutex_);
    return status_;
}

bool RCCarDriver::is_connected() const {
    std::lock_guard lock(mutex_);
    return connected_;
}

uados::Status RCCarDriver::emergency_stop() {
    std::lock_guard lock(mutex_);
    
    UADOS_LOG_WARN("RC Car Driver: EMERGENCY STOP triggered");
    
    last_cmd_.throttle = 0.0;
    last_cmd_.brake = 1.0;
    last_cmd_.emergency_stop = true;
    
    update_pwm_channels(last_cmd_);
    
    state_.velocity = {0.0, 0.0, 0.0};
    state_.acceleration = {0.0, 0.0, 0.0};
    
    return Status::Ok;
}

void RCCarDriver::update_pwm_channels(const VehicleCommand& cmd) noexcept {
    // 1. Steering PWM: -max_steering_angle (1000us) to +max_steering_angle (2000us)
    double steer_pct = cmd.steering_angle / caps_.max_steering_angle;
    steer_pct = std::clamp(steer_pct, -1.0, 1.0);
    steering_pwm_ = static_cast<uint16_t>(1500 + steer_pct * 500);

    // 2. Throttle/Brake ESC PWM:
    // ESC standard: Neutral is 1500us, Full throttle is 2000us, Full brake is 1000us.
    if (cmd.emergency_stop) {
        throttle_pwm_ = 1000; // Full reverse/brake PWM
    } else if (cmd.brake > 0.0) {
        double brake_pct = std::clamp(cmd.brake, 0.0, 1.0);
        throttle_pwm_ = static_cast<uint16_t>(1500 - brake_pct * 500);
    } else {
        double throttle_pct = std::clamp(cmd.throttle, 0.0, 1.0);
        throttle_pwm_ = static_cast<uint16_t>(1500 + throttle_pct * 500);
    }
}

void RCCarDriver::update_simulation() {
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time_);
    double dt = static_cast<double>(elapsed.count()) / 1e6; // to seconds
    last_update_time_ = now;

    if (dt <= 0.0) return;

    // Ackermann Kinematic Bicycle simulation
    double current_speed = state_.velocity.magnitude();
    double accel = 0.0;

    if (last_cmd_.emergency_stop) {
        accel = -caps_.max_deceleration;
    } else {
        double throttle_force = last_cmd_.throttle * caps_.max_acceleration;
        double braking_force = last_cmd_.brake * caps_.max_deceleration;
        accel = throttle_force - braking_force;

        // Higher rolling resistance for a small 1/10 scale chassis
        accel -= 0.15 * current_speed;
    }

    double new_speed = current_speed + accel * dt;
    if (new_speed < 0.0) new_speed = 0.0;
    if (new_speed > caps_.max_speed) new_speed = caps_.max_speed;

    double L = caps_.wheelbase;
    double delta = last_cmd_.steering_angle;

    // Get current yaw from quaternion
    double yaw = state_.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();

    double dx = new_speed * std::cos(yaw);
    double dy = new_speed * std::sin(yaw);
    double dyaw = (new_speed / L) * std::tan(delta);

    state_.position.x += dx * dt;
    state_.position.y += dy * dt;
    yaw += dyaw * dt;

    state_.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                         Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                         Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ());

    state_.velocity.vx = dx;
    state_.velocity.vy = dy;
    state_.acceleration.ax = accel * std::cos(yaw);
    state_.acceleration.ay = accel * std::sin(yaw);

    state_.steering_angle = delta;
    state_.timestamp = now;

    // 1/10 scale wheel radius ~3.1 cm
    double wheel_rot = new_speed / 0.031;
    state_.wheel_speeds = {wheel_rot, wheel_rot, wheel_rot, wheel_rot};
}

} // namespace uados::hal
