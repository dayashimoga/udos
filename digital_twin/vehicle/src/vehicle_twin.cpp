#include "uados/digital_twin/vehicle_twin.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::digital_twin {

UADOS_DECLARE_LOGGER("digital_twin.vehicle")

Status VehicleDigitalTwin::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Vehicle Digital Twin Simulator...");

    if (config) {
        if (config["wheelbase"]) {
            wheelbase_ = config["wheelbase"].as<double>();
        }
        if (config["cg_to_rear"]) {
            lr_ = config["cg_to_rear"].as<double>();
        }
        if (config["max_speed"]) {
            max_speed_ = config["max_speed"].as<double>();
        }
    }

    x_ = 0.0;
    y_ = 0.0;
    yaw_ = 0.0;
    v_ = 0.0;

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Vehicle Digital Twin configured with Wheelbase={:.2f}m, Lr={:.2f}m", wheelbase_, lr_);
    return Status::Ok;
}

Status VehicleDigitalTwin::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status VehicleDigitalTwin::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void VehicleDigitalTwin::step(double steer, double accel, double dt) noexcept {
    std::lock_guard lock(mutex_);

    if (!active_ || dt <= 0.0001) {
        return;
    }

    // Clamp input actuators to safe physical limits
    double delta = std::clamp(steer, -0.70, 0.70); // Max steering angle clamp
    
    // 1. Calculate lateral side-slip angle (beta) at Center of Gravity
    double beta = 0.0;
    if (std::abs(delta) > 0.0001) {
        beta = std::atan2(lr_ * std::tan(delta), wheelbase_);
    }

    // 2. Kinematic Bicycle dynamics differential equations
    double dx = v_ * std::cos(yaw_ + beta);
    double dy = v_ * std::sin(yaw_ + beta);
    double dyaw = 0.0;
    
    if (wheelbase_ > 0.1) {
        dyaw = (v_ / wheelbase_) * std::cos(beta) * std::tan(delta);
    }

    // 3. Integrate state updates
    x_ += dx * dt;
    y_ += dy * dt;
    yaw_ += dyaw * dt;
    
    // Normalize yaw heading to [-pi, pi]
    while (yaw_ > M_PI)  yaw_ -= 2.0 * M_PI;
    while (yaw_ < -M_PI) yaw_ += 2.0 * M_PI;

    v_ = std::clamp(v_ + accel * dt, 0.0, max_speed_);
}

void VehicleDigitalTwin::reset(const Pose& pose, double initial_speed) noexcept {
    std::lock_guard lock(mutex_);
    x_ = pose.position.x;
    y_ = pose.position.y;
    
    double extract_yaw = pose.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    if (std::isnan(extract_yaw)) {
        extract_yaw = 0.0;
    }
    yaw_ = extract_yaw;
    v_ = initial_speed;

    UADOS_LOG_INFO("Vehicle Digital Twin reset to Pose=({:.2f}, {:.2f}, heading={:.2f} rad), Speed={:.2f} m/s",
                   x_, y_, yaw_, v_);
}

VehicleState VehicleDigitalTwin::get_state() const noexcept {
    std::lock_guard lock(mutex_);

    VehicleState state;
    state.timestamp = Clock::now();
    
    // 3D position
    state.position.x = x_;
    state.position.y = y_;
    state.position.z = 0.0;

    // 3D orientation
    state.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                        Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                        Eigen::AngleAxisd(yaw_, Eigen::Vector3d::UnitZ());

    // Fused velocities
    state.velocity.vx = v_;
    state.velocity.vy = 0.0; // simple lateral slip feedback projected on ego axis
    state.velocity.vz = 0.0;

    state.acceleration.ax = 0.0; // derivative of v
    state.acceleration.ay = 0.0;
    state.acceleration.az = 0.0;

    state.steering_angle = 0.0;
    state.engine_running = active_;
    state.gear = GearPosition::Drive;

    return state;
}

} // namespace uados::digital_twin
