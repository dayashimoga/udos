#include "uados/localization/slam_engine.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::localization {

UADOS_DECLARE_LOGGER("localization.slam")

Status SLAMEngine::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing SLAM dead-reckoning engine...");

    current_pose_.position = {0.0, 0.0, 0.0};
    current_pose_.orientation = Quat::Identity();
    current_pose_.timestamp = Clock::now();
    yaw_ = 0.0;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    return Status::Ok;
}

Status SLAMEngine::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status SLAMEngine::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void SLAMEngine::reset_pose(const Pose& pose) {
    std::lock_guard lock(mutex_);
    current_pose_ = pose;
    yaw_ = pose.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    UADOS_LOG_INFO("SLAM: Odometry pose reset to: ({:.2f}, {:.2f}), yaw={:.2f} rad",
                   pose.position.x, pose.position.y, yaw_);
}

Pose SLAMEngine::update_odometry(double linear_velocity, double angular_velocity, double dt) {
    std::lock_guard lock(mutex_);
    if (!active_ || dt <= 0.0) return current_pose_;

    // 1. Integrate yaw: yaw = yaw + yaw_rate * dt
    yaw_ += angular_velocity * dt;

    // Keep yaw normalized between -PI and PI
    if (yaw_ > 3.1415926535) yaw_ -= 2.0 * 3.1415926535;
    if (yaw_ < -3.1415926535) yaw_ += 2.0 * 3.1415926535;

    // 2. Integrate position:
    // dx = speed * cos(yaw) * dt
    // dy = speed * sin(yaw) * dt
    double dx = linear_velocity * std::cos(yaw_) * dt;
    double dy = linear_velocity * std::sin(yaw_) * dt;

    current_pose_.position.x += dx;
    current_pose_.position.y += dy;

    // 3. Update Quaternion
    current_pose_.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                                Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                                Eigen::AngleAxisd(yaw_, Eigen::Vector3d::UnitZ());

    current_pose_.timestamp = Clock::now();

    UADOS_LOG_DEBUG("SLAM Dead Reckoning: pos=({:.2f}, {:.2f}), yaw={:.2f} rad",
                    current_pose_.position.x, current_pose_.position.y, yaw_);

    return current_pose_;
}

Pose SLAMEngine::current_pose() const {
    std::lock_guard lock(mutex_);
    return current_pose_;
}

} // namespace uados::localization
