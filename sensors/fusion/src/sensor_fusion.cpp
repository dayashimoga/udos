#include "uados/sensors/sensor_fusion.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::sensors {

UADOS_DECLARE_LOGGER("sensors.fusion")

Status SensorFusion::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Sensor Fusion EKF Engine...");

    if (config) {
        if (config["origin_latitude"]) {
            origin_lat_ = config["origin_latitude"].as<double>();
        }
        if (config["origin_longitude"]) {
            origin_lon_ = config["origin_longitude"].as<double>();
        }
    }

    // Initialize EKF states and matrices
    x_ = Eigen::Vector4d::Zero();

    // P matrix: standard initial uncertainty (large)
    P_ = Eigen::Matrix4d::Identity() * 10.0;

    // Q matrix: Process noise (uncertainty in motion model)
    Q_ = Eigen::Matrix4d::Zero();
    Q_(0, 0) = 0.05; // position noise
    Q_(1, 1) = 0.05;
    Q_(2, 2) = 0.2;  // velocity noise
    Q_(3, 3) = 0.2;

    // R matrix: Measurement noise (uncertainty in GPSRTK readings)
    R_ = Eigen::Matrix2d::Identity() * 0.04; // 20cm uncertainty squared

    initialized_ = false;
    active_ = false;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Sensor Fusion EKF Engine initialized at origin: {:.6f}, {:.6f}", origin_lat_, origin_lon_);
    return Status::Ok;
}

Status SensorFusion::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    last_prediction_time_ = Clock::now();
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status SensorFusion::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void SensorFusion::predict(const IMUReading& imu) {
    std::lock_guard lock(mutex_);
    if (!active_) return;

    auto now = imu.timestamp;
    
    if (!initialized_) {
        // Bootstrap EKF using the first prediction step
        last_prediction_time_ = now;
        initialized_ = true;
        return;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_prediction_time_);
    double dt = static_cast<double>(elapsed.count()) / 1e6; // to seconds
    last_prediction_time_ = now;

    if (dt <= 0.0) return;

    // 1. Setup motion transition matrix F
    Eigen::Matrix4d F = Eigen::Matrix4d::Identity();
    F(0, 2) = dt;
    F(1, 3) = dt;

    // 2. Setup control input B and vector u
    // Convert IMU acceleration (excluding local gravity in z)
    // For simplicity, EKF models simple 2D longitudinal/lateral dynamics
    double ax_world = imu.linear_acceleration.ax;
    double ay_world = imu.linear_acceleration.ay;

    Eigen::Matrix<double, 4, 2> B;
    B << 0.5 * dt * dt, 0.0,
         0.0,           0.5 * dt * dt,
         dt,            0.0,
         0.0,           dt;

    Eigen::Vector2d u(ax_world, ay_world);

    // 3. Predict state: x_ = F * x_ + B * u
    x_ = F * x_ + B * u;

    // 4. Predict covariance: P_ = F * P_ * F^T + Q_
    P_ = F * P_ * F.transpose() + Q_;
}

void SensorFusion::correct(const GPSFix& gps) {
    std::lock_guard lock(mutex_);
    if (!active_) return;

    if (!initialized_) {
        // Initialize state directly from first GPS fix
        double zx = (gps.coordinate.longitude - origin_lon_) * 111320.0 * std::cos(origin_lat_ * 3.14159 / 180.0);
        double zy = (gps.coordinate.latitude - origin_lat_) * 110540.0;
        x_(0) = zx;
        x_(1) = zy;
        x_(2) = gps.speed * std::cos(gps.heading);
        x_(3) = gps.speed * std::sin(gps.heading);
        
        last_prediction_time_ = gps.timestamp;
        initialized_ = true;
        return;
    }

    // 1. Project GPS Lat/Lon into local tangent plane coordinates (UTM approximation)
    double zx = (gps.coordinate.longitude - origin_lon_) * 111320.0 * std::cos(origin_lat_ * 3.14159 / 180.0);
    double zy = (gps.coordinate.latitude - origin_lat_) * 110540.0;
    Eigen::Vector2d z(zx, zy);

    // 2. Setup measurement matrix H
    Eigen::Matrix<double, 2, 4> H;
    H << 1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0;

    // Update R dynamically from GPS reported precision
    R_(0, 0) = gps.horizontal_accuracy * gps.horizontal_accuracy;
    R_(1, 1) = gps.horizontal_accuracy * gps.horizontal_accuracy;

    // 3. Compute residual: y = z - H * x
    Eigen::Vector2d y = z - H * x_;

    // 4. Compute residual covariance: S = H * P * H^T + R
    Eigen::Matrix2d S = H * P_ * H.transpose() + R_;

    // 5. Compute Kalman Gain: K = P * H^T * S^-1
    Eigen::Matrix<double, 4, 2> K = P_ * H.transpose() * S.inverse();

    // 6. Update state: x = x + K * y
    x_ = x_ + K * y;

    // 7. Update covariance: P = (I - K * H) * P
    Eigen::Matrix4d I = Eigen::Matrix4d::Identity();
    P_ = (I - K * H) * P_;

    UADOS_LOG_DEBUG("EKF Correction: GPS=({:.2f}, {:.2f}), Fused State=({:.2f}, {:.2f})", zx, zy, x_(0), x_(1));
}

KinematicState SensorFusion::state_estimate() const {
    std::lock_guard lock(mutex_);

    KinematicState state;
    state.pose.timestamp = last_prediction_time_;
    state.pose.position.x = x_(0);
    state.pose.position.y = x_(1);
    state.pose.position.z = 0.0;

    // Compute heading yaw from velocity vector
    double yaw = std::atan2(x_(3), x_(2));
    state.pose.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                             Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                             Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ());

    state.velocity.vx = x_(2);
    state.velocity.vy = x_(3);
    state.velocity.vz = 0.0;

    return state;
}

} // namespace uados::sensors
