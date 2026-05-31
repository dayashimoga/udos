#pragma once

/// @file sensor_fusion.hpp
/// @brief Extended Kalman Filter for real-time 6-DOF state estimation.

#include "uados/sensors/sensor.hpp"
#include <mutex>
#include <Eigen/Core>
#include <Eigen/Dense>

namespace uados::sensors {

/// @brief Fuses GPS and IMU sensor streams using a Kalman Filter.
///
/// Implements a 4-state Kalman Filter tracking absolute position (px, py)
/// and velocity (vx, vy), predicting state using high-frequency IMU
/// accelerations and correcting state using low-frequency absolute GNSS updates.
class SensorFusion final : public uados::core::ComponentBase {
public:
    SensorFusion() = default;
    ~SensorFusion() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "sensors.fusion"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- Filter Methods --

    /// Perform EKF prediction step using IMU acceleration telemetry
    /// @param imu Linear accelerations and angular velocities
    void predict(const IMUReading& imu);

    /// Perform EKF correction step using absolute GNSS coordinate updates
    /// @param gps Lat/lon coordinates with RTK precision
    void correct(const GPSFix& gps);

    /// Get current state estimate
    [[nodiscard]] KinematicState state_estimate() const;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // EKF Matrices
    // State vector: x = [px, py, vx, vy]^T
    Eigen::Vector4d x_{Eigen::Vector4d::Zero()};
    // Covariance matrix
    Eigen::Matrix4d P_{Eigen::Matrix4d::Identity()};
    // Process noise covariance
    Eigen::Matrix4d Q_{Eigen::Matrix4d::Identity()};
    // Measurement noise covariance
    Eigen::Matrix2d R_{Eigen::Matrix2d::Identity()};

    // Reference origin for local tangent plane GPS coordinate projections
    double origin_lat_{37.7749};
    double origin_lon_{-122.4194};

    Timestamp last_prediction_time_;
    bool initialized_{false};
};

} // namespace uados::sensors
