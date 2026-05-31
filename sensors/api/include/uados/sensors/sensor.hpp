#pragma once

/// @file sensor.hpp
/// @brief Unified sensor interface for all sensor types.
///
/// All sensors (camera, LiDAR, radar, GPS, IMU) implement this
/// interface, providing a consistent API for sensor data acquisition,
/// calibration, health monitoring, and recording.

#include "uados/types.hpp"
#include "uados/component.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uados::sensors {

// ============================================================================
// Sensor Data Types
// ============================================================================

/// Base class for all sensor data
struct SensorData {
    virtual ~SensorData() = default;

    Timestamp timestamp{};           ///< Data acquisition time
    uint64_t sequence{0};            ///< Monotonic sequence number
    SensorType sensor_type{SensorType::Unknown};
    std::string sensor_id;           ///< Unique sensor identifier
};

/// Camera image frame
struct ImageFrame : public SensorData {
    uint32_t width{0};
    uint32_t height{0};
    uint32_t channels{3};           ///< 1=mono, 3=RGB, 4=RGBA
    uint32_t bytes_per_pixel{3};
    std::vector<uint8_t> data;      ///< Raw pixel data (row-major)

    [[nodiscard]] size_t size_bytes() const noexcept {
        return static_cast<size_t>(width) * height * bytes_per_pixel;
    }
};

/// Single LiDAR point
struct LiDARPoint {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float intensity{0.0f};
    uint16_t ring{0};               ///< Laser ring index
    float timestamp_offset{0.0f};   ///< Time offset from scan start (s)
};

/// LiDAR point cloud
struct PointCloud : public SensorData {
    std::vector<LiDARPoint> points;
    uint32_t width{0};              ///< Organized cloud width (0=unorganized)
    uint32_t height{0};             ///< Organized cloud height
};

/// Single radar detection
struct RadarDetection {
    float range{0.0f};             ///< Distance (m)
    float azimuth{0.0f};           ///< Horizontal angle (rad)
    float elevation{0.0f};         ///< Vertical angle (rad)
    float velocity{0.0f};          ///< Radial velocity (m/s)
    float rcs{0.0f};               ///< Radar cross-section (dBsm)
    float snr{0.0f};               ///< Signal-to-noise ratio (dB)
};

/// Radar scan (collection of detections)
struct RadarScan : public SensorData {
    std::vector<RadarDetection> detections;
};

/// GPS fix
struct GPSFix : public SensorData {
    GeoCoordinate coordinate;
    Scalar horizontal_accuracy{0.0};  ///< Horizontal accuracy (m)
    Scalar vertical_accuracy{0.0};    ///< Vertical accuracy (m)
    Scalar speed{0.0};                ///< Ground speed (m/s)
    Scalar heading{0.0};              ///< True heading (rad)
    uint8_t satellites{0};            ///< Number of satellites
    enum class FixType : uint8_t {
        NoFix, Fix2D, Fix3D, DGPS, RTK_Float, RTK_Fixed
    } fix_type{FixType::NoFix};
};

/// IMU reading
struct IMUReading : public SensorData {
    Acceleration3D linear_acceleration;  ///< m/s²
    Vec3 angular_velocity;               ///< rad/s (x, y, z)
    Quat orientation{Quat::Identity()};  ///< Estimated orientation
    Scalar temperature{0.0};             ///< Sensor temperature (°C)
};

// ============================================================================
// Sensor Configuration
// ============================================================================

/// Sensor configuration
struct SensorConfig {
    std::string sensor_id;           ///< Unique sensor identifier
    SensorType type{SensorType::Unknown};
    Extrinsics extrinsics;           ///< Mount position/orientation
    Scalar data_rate_hz{0.0};        ///< Expected data rate
    uados::core::Config params;      ///< Type-specific parameters
};

// ============================================================================
// Sensor Interface
// ============================================================================

/// @brief Unified sensor interface for all sensor types.
///
/// Implementations:
/// - CARLA sensor drivers (camera, LiDAR, radar, GPS, IMU)
/// - RC car sensors (RealSense, RPLiDAR, GPS module, IMU)
/// - Production vehicle sensors (industrial cameras, Velodyne, etc.)
class ISensor : public uados::core::ComponentBase {
public:
    ~ISensor() override = default;

    /// Read the latest sensor data
    /// @return Sensor data (type depends on sensor)
    [[nodiscard]] virtual std::shared_ptr<SensorData> read() = 0;

    /// Get the sensor type
    [[nodiscard]] virtual SensorType type() const = 0;

    /// Get the sensor's unique identifier
    [[nodiscard]] virtual std::string_view sensor_id() const = 0;

    /// Get the extrinsic calibration
    [[nodiscard]] virtual Extrinsics extrinsics() const = 0;

    /// Set the extrinsic calibration
    virtual void set_extrinsics(const Extrinsics& ext) = 0;

    /// Get the sensor's health
    [[nodiscard]] virtual SensorHealth sensor_health() const = 0;

    /// Check if the sensor is producing data
    [[nodiscard]] virtual bool is_active() const = 0;
};

} // namespace uados::sensors
