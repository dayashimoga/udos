#include "uados/localization/pose_estimator.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::localization {

UADOS_DECLARE_LOGGER("localization.pose")

constexpr double kEarthRadiusMeters = 6378137.0;
constexpr double kPi = 3.14159265358979323846;

Status PoseEstimator::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Pose Estimator...");

    if (config) {
        if (config["origin_latitude"]) {
            origin_lat_ = config["origin_latitude"].as<double>();
        }
        if (config["origin_longitude"]) {
            origin_lon_ = config["origin_longitude"].as<double>();
        }
    }

    current_pose_.position = {0.0, 0.0, 0.0};
    current_pose_.orientation = Quat::Identity();
    current_pose_.timestamp = Clock::now();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Pose Estimator initialized. local ENU origin set to: {:.6f}, {:.6f}", origin_lat_, origin_lon_);
    return Status::Ok;
}

Status PoseEstimator::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status PoseEstimator::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void PoseEstimator::geodesic_to_local(double lat, double lon, double /*alt*/, double& x, double& y) const noexcept {
    double dlat = (lat - origin_lat_) * kPi / 180.0;
    double dlon = (lon - origin_lon_) * kPi / 180.0;

    // Local ENU plane projection
    y = dlat * kEarthRadiusMeters;
    x = dlon * kEarthRadiusMeters * std::cos(origin_lat_ * kPi / 180.0);
}

void PoseEstimator::local_to_geodesic(double x, double y, double& lat, double& lon) const noexcept {
    double dlat = y / kEarthRadiusMeters;
    double dlon = x / (kEarthRadiusMeters * std::cos(origin_lat_ * kPi / 180.0));

    lat = origin_lat_ + (dlat * 180.0 / kPi);
    lon = origin_lon_ + (dlon * 180.0 / kPi);
}

void PoseEstimator::update_pose(const Pose& pose) {
    std::lock_guard lock(mutex_);
    current_pose_ = pose;
}

Pose PoseEstimator::current_pose() const {
    std::lock_guard lock(mutex_);
    return current_pose_;
}

} // namespace uados::localization
