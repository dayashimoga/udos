#include "uados/digital_twin/sensor_twin.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::digital_twin {

UADOS_DECLARE_LOGGER("digital_twin.sensor")

Status SensorTwin::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Sensor Digital Twin Simulator...");

    if (config) {
        if (config["camera_fx"]) fx_ = config["camera_fx"].as<double>();
        if (config["camera_fy"]) fy_ = config["camera_fy"].as<double>();
        if (config["camera_cx"]) cx_ = config["camera_cx"].as<double>();
        if (config["camera_cy"]) cy_ = config["camera_cy"].as<double>();
        if (config["camera_width"]) width_ = config["camera_width"].as<int>();
        if (config["camera_height"]) height_ = config["camera_height"].as<int>();

        if (config["radar_range_limit"]) {
            radar_range_limit_ = config["radar_range_limit"].as<double>();
        }
        if (config["radar_fov_rad"]) {
            radar_fov_rad_ = config["radar_fov_rad"].as<double>();
        }
        if (config["noise_range_std"]) {
            noise_range_std_ = config["noise_range_std"].as<double>();
        }
        if (config["noise_velocity_std"]) {
            noise_velocity_std_ = config["noise_velocity_std"].as<double>();
        }
    }

    generator_.seed(rd_());

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Sensor Digital Twin initialized. Camera: {}x{} (fx={:.1f}), Radar range: {:.1f}m",
                   width_, height_, fx_, radar_range_limit_);
    return Status::Ok;
}

Status SensorTwin::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status SensorTwin::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

PixelPoint SensorTwin::project_to_camera(const Position3D& point_3d, const Pose& camera_pose) const noexcept {
    std::lock_guard lock(mutex_);

    PixelPoint pixel;
    pixel.visible = false;

    // Camera center relative offset in vehicle coordinate space
    double cam_x = camera_pose.position.x;
    double cam_y = camera_pose.position.y;
    double cam_z = camera_pose.position.z;

    // Transform relative coordinate offset (Vehicle Space -> Camera Optical Frame)
    // optical frame: Z forward, X right, Y down
    double dx = point_3d.x - cam_x; // forward
    double dy = -(point_3d.y - cam_y); // left to right
    double dz = -(point_3d.z - cam_z); // up to down

    double z_opt = dx;
    double x_opt = dy;
    double y_opt = dz;

    if (z_opt <= 0.1) {
        return pixel; // behind camera view
    }

    // Pinhole camera intrinsic mapping equations
    pixel.u = fx_ * (x_opt / z_opt) + cx_;
    pixel.v = fy_ * (y_opt / z_opt) + cy_;

    // Check visibility inside frame bounds
    if (pixel.u >= 0.0 && pixel.u <= static_cast<double>(width_) &&
        pixel.v >= 0.0 && pixel.v <= static_cast<double>(height_)) {
        pixel.visible = true;
    }

    return pixel;
}

std::vector<DetectedObject> SensorTwin::simulate_radar(
    const VehicleState& ego_state,
    const std::vector<DetectedObject>& traffic_agents) noexcept {
    std::lock_guard lock(mutex_);

    if (!active_) {
        return {};
    }

    std::vector<DetectedObject> scanned_objects;
    std::normal_distribution<double> dist_range(0.0, noise_range_std_);
    std::normal_distribution<double> dist_vel(0.0, noise_velocity_std_);

    double ego_x = ego_state.position.x;
    double ego_y = ego_state.position.y;
    double yaw_ego = ego_state.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    if (std::isnan(yaw_ego)) {
        yaw_ego = 0.0;
    }

    for (const auto& agent : traffic_agents) {
        double dx = agent.position.x - ego_x;
        double dy = agent.position.y - ego_y;

        // Transform error vector to local vehicle frame coordinates
        double dx_local = dx * std::cos(yaw_ego) + dy * std::sin(yaw_ego);
        double dy_local = -dx * std::sin(yaw_ego) + dy * std::cos(yaw_ego);

        // Compute Range and Azimuth angle
        double range = std::sqrt(dx_local * dx_local + dy_local * dy_local);
        double azimuth = std::atan2(dy_local, dx_local);

        // Radar FOV and range checks
        if (range > radar_range_limit_ || std::abs(azimuth) > radar_fov_rad_) {
            continue; // outside radar cone
        }

        // Add Gaussian noise modeling
        double noisy_range = range + dist_range(generator_);
        double noisy_azimuth = azimuth; // negligible angle noise for target tracks

        // Reconstruct local coordinates from noisy readings
        double noisy_dx_local = noisy_range * std::cos(noisy_azimuth);
        double noisy_dy_local = noisy_range * std::sin(noisy_azimuth);

        // Convert local coordinates back to world coordinates
        double noisy_x_world = ego_x + (noisy_dx_local * std::cos(yaw_ego) - noisy_dy_local * std::sin(yaw_ego));
        double noisy_y_world = ego_y + (noisy_dx_local * std::sin(yaw_ego) + noisy_dy_local * std::cos(yaw_ego));

        DetectedObject detected_target;
        detected_target.id = agent.id;
        detected_target.object_class = agent.object_class;
        
        // Degrade track confidence as range increases
        double conf = agent.confidence * (1.0 - (range / radar_range_limit_) * 0.3);
        detected_target.confidence = std::clamp(conf, 0.01, 1.0);

        detected_target.position.x = noisy_x_world;
        detected_target.position.y = noisy_y_world;
        detected_target.position.z = 0.0;
        
        // Add velocity noise
        double noisy_vx = agent.velocity.vx + dist_vel(generator_);
        detected_target.velocity.vx = noisy_vx;
        detected_target.velocity.vy = agent.velocity.vy;
        detected_target.velocity.vz = 0.0;

        detected_target.dimensions = agent.dimensions;
        detected_target.orientation = agent.orientation;
        detected_target.timestamp = Clock::now();

        scanned_objects.push_back(detected_target);
    }

    UADOS_LOG_DEBUG("Radar Simulator: Scanned {} dynamic targets inside FOV.", scanned_objects.size());
    return scanned_objects;
}

} // namespace uados::digital_twin
