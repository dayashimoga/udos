#pragma once

/// @file types.hpp
/// @brief Core type definitions used throughout UADOS.
///
/// This header defines the fundamental data types for the autonomy stack:
/// timestamps, coordinates, vehicle state, sensor data structures, and
/// status codes. All components depend on these shared types.

#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace uados {

// ============================================================================
// Time Types
// ============================================================================

/// High-resolution clock for all timing
using Clock = std::chrono::steady_clock;

/// Timestamp type (nanosecond precision)
using Timestamp = std::chrono::time_point<Clock>;

/// Duration type
using Duration = std::chrono::nanoseconds;

/// Wall-clock time (for logging and external communication)
using WallClock = std::chrono::system_clock;
using WallTimestamp = std::chrono::time_point<WallClock>;

// ============================================================================
// Numeric Types
// ============================================================================

/// Default floating-point precision (double for safety-critical math)
using Scalar = double;

/// 2D vector
using Vec2 = Eigen::Vector2d;

/// 3D vector
using Vec3 = Eigen::Vector3d;

/// 4D vector
using Vec4 = Eigen::Vector4d;

/// 3x3 matrix
using Mat3 = Eigen::Matrix3d;

/// 4x4 matrix
using Mat4 = Eigen::Matrix4d;

/// Quaternion (for orientation)
using Quat = Eigen::Quaterniond;

// ============================================================================
// Spatial Types
// ============================================================================

/// 3D position in world frame (meters)
struct Position3D {
    Scalar x{0.0};  ///< East (m)
    Scalar y{0.0};  ///< North (m)
    Scalar z{0.0};  ///< Up (m)

    [[nodiscard]] Vec3 to_vec3() const noexcept { return {x, y, z}; }
    static Position3D from_vec3(const Vec3& v) noexcept { return {v.x(), v.y(), v.z()}; }
};

/// 3D velocity (m/s)
struct Velocity3D {
    Scalar vx{0.0};
    Scalar vy{0.0};
    Scalar vz{0.0};

    [[nodiscard]] Scalar magnitude() const noexcept {
        return std::sqrt(vx * vx + vy * vy + vz * vz);
    }
    [[nodiscard]] Vec3 to_vec3() const noexcept { return {vx, vy, vz}; }
};

/// 3D acceleration (m/s²)
struct Acceleration3D {
    Scalar ax{0.0};
    Scalar ay{0.0};
    Scalar az{0.0};

    [[nodiscard]] Scalar magnitude() const noexcept {
        return std::sqrt(ax * ax + ay * ay + az * az);
    }
    [[nodiscard]] Vec3 to_vec3() const noexcept { return {ax, ay, az}; }
};

/// Euler angles (radians)
struct EulerAngles {
    Scalar roll{0.0};   ///< Rotation about X axis
    Scalar pitch{0.0};  ///< Rotation about Y axis
    Scalar yaw{0.0};    ///< Rotation about Z axis
};

/// Full 6-DOF pose
struct Pose {
    Position3D position;
    Quat orientation{Quat::Identity()};
    Timestamp timestamp{};

    /// Get the 4x4 transformation matrix
    [[nodiscard]] Mat4 to_matrix() const noexcept {
        Mat4 m = Mat4::Identity();
        m.block<3, 3>(0, 0) = orientation.toRotationMatrix();
        m(0, 3) = position.x;
        m(1, 3) = position.y;
        m(2, 3) = position.z;
        return m;
    }
};

/// Pose with velocity (for tracking and prediction)
struct KinematicState {
    Pose pose;
    Velocity3D velocity;
    Acceleration3D acceleration;
    Scalar yaw_rate{0.0};  ///< rad/s
};

// ============================================================================
// Geographic Types
// ============================================================================

/// WGS84 coordinates
struct GeoCoordinate {
    Scalar latitude{0.0};   ///< degrees
    Scalar longitude{0.0};  ///< degrees
    Scalar altitude{0.0};   ///< meters above ellipsoid
};

// ============================================================================
// Vehicle Types
// ============================================================================

/// Gear position
enum class GearPosition : uint8_t {
    Park,
    Reverse,
    Neutral,
    Drive,
    Low,
    Manual1,
    Manual2,
    Manual3,
    Unknown
};

/// Vehicle state (read from vehicle hardware)
struct VehicleState {
    Timestamp timestamp{};
    Position3D position;
    Velocity3D velocity;
    Acceleration3D acceleration;
    Quat orientation{Quat::Identity()};
    Scalar steering_angle{0.0};              ///< Current steering angle (rad)
    std::array<Scalar, 4> wheel_speeds{};    ///< FL, FR, RL, RR (rad/s)
    GearPosition gear{GearPosition::Unknown};
    Scalar battery_voltage{0.0};             ///< Battery voltage (V)
    bool engine_running{false};
};

/// Vehicle command (sent to vehicle hardware)
struct VehicleCommand {
    Timestamp timestamp{};
    Scalar steering_angle{0.0};  ///< Desired steering angle (rad)
    Scalar throttle{0.0};        ///< Throttle position [0.0, 1.0]
    Scalar brake{0.0};           ///< Brake pressure [0.0, 1.0]
    GearPosition gear{GearPosition::Drive};
    bool emergency_stop{false};  ///< Emergency stop flag
};

/// Vehicle capabilities (reported by driver)
struct VehicleCapabilities {
    Scalar max_steering_angle{0.0};    ///< Maximum steering angle (rad)
    Scalar max_speed{0.0};             ///< Maximum speed (m/s)
    Scalar max_acceleration{0.0};      ///< Maximum acceleration (m/s²)
    Scalar max_deceleration{0.0};      ///< Maximum deceleration (m/s²)
    Scalar wheelbase{0.0};             ///< Wheelbase (m)
    Scalar track_width{0.0};           ///< Track width (m)
    bool has_steering{false};
    bool has_throttle{false};
    bool has_brake{false};
    bool has_gear{false};
};

// ============================================================================
// Status Types
// ============================================================================

/// General operation status
enum class Status : uint8_t {
    Ok,
    Error,
    Timeout,
    NotReady,
    NotSupported,
    InvalidArgument,
    ResourceExhausted,
    PermissionDenied,
    AlreadyExists,
    NotFound,
    Cancelled,
    Unknown
};

/// Convert Status to string
[[nodiscard]] constexpr std::string_view status_to_string(Status s) noexcept {
    switch (s) {
        case Status::Ok:                return "Ok";
        case Status::Error:             return "Error";
        case Status::Timeout:           return "Timeout";
        case Status::NotReady:          return "NotReady";
        case Status::NotSupported:      return "NotSupported";
        case Status::InvalidArgument:   return "InvalidArgument";
        case Status::ResourceExhausted: return "ResourceExhausted";
        case Status::PermissionDenied:  return "PermissionDenied";
        case Status::AlreadyExists:     return "AlreadyExists";
        case Status::NotFound:          return "NotFound";
        case Status::Cancelled:         return "Cancelled";
        case Status::Unknown:           return "Unknown";
    }
    return "Unknown";
}

/// Result type combining value and status
template <typename T>
struct Result {
    Status status{Status::Ok};
    std::optional<T> value;
    std::string message;

    [[nodiscard]] bool ok() const noexcept { return status == Status::Ok; }
    [[nodiscard]] const T& operator*() const { return value.value(); }
    [[nodiscard]] T& operator*() { return value.value(); }

    static Result success(T val) {
        return {Status::Ok, std::move(val), {}};
    }

    static Result error(Status s, std::string msg = {}) {
        return {s, std::nullopt, std::move(msg)};
    }
};

// ============================================================================
// Component Types
// ============================================================================

/// Component health status
enum class HealthStatus : uint8_t {
    Healthy,
    Degraded,
    Unhealthy,
    Unknown
};

/// Component lifecycle state
enum class ComponentState : uint8_t {
    Loaded,
    Initialized,
    Running,
    Paused,
    Stopping,
    Stopped,
    Error
};

/// Unique component identifier
using ComponentId = std::string;

/// Unique subscription identifier
using SubscriptionId = uint64_t;

/// Unique task identifier
using TaskId = uint64_t;

/// Unique plugin identifier
using PluginId = std::string;

// ============================================================================
// Sensor Types
// ============================================================================

/// Sensor type enumeration
enum class SensorType : uint8_t {
    Camera,
    LiDAR,
    Radar,
    GPS,
    IMU,
    Ultrasonic,
    Wheel_Encoder,
    Unknown
};

/// Sensor health
struct SensorHealth {
    HealthStatus status{HealthStatus::Unknown};
    Scalar data_rate_hz{0.0};       ///< Current data rate
    Scalar expected_rate_hz{0.0};   ///< Expected data rate
    uint64_t dropped_frames{0};     ///< Total dropped frames
    Timestamp last_data{};          ///< Timestamp of last received data
};

/// 6-DOF extrinsic calibration (sensor mount position/orientation)
struct Extrinsics {
    Pose mount_pose;  ///< Sensor position/orientation in vehicle frame
};

// ============================================================================
// Perception Types
// ============================================================================

/// Object class
enum class ObjectClass : uint8_t {
    Unknown,
    Car,
    Truck,
    Bus,
    Motorcycle,
    Bicycle,
    Pedestrian,
    Animal,
    TrafficSign,
    TrafficLight,
    TrafficCone,
    Barrier,
    Other
};

/// Detected object
struct DetectedObject {
    uint64_t id{0};                ///< Unique object ID
    ObjectClass object_class{ObjectClass::Unknown};
    Scalar confidence{0.0};        ///< Detection confidence [0, 1]
    Position3D position;           ///< Center position in world frame
    Vec3 dimensions;               ///< Length, width, height (m)
    Quat orientation{Quat::Identity()};
    Velocity3D velocity;           ///< Estimated velocity
    Timestamp timestamp{};
};

/// Traffic light state
enum class TrafficLightState : uint8_t {
    Unknown,
    Red,
    Yellow,
    Green,
    RedYellow,
    FlashingRed,
    FlashingYellow,
    Off
};

// ============================================================================
// Planning Types
// ============================================================================

/// Trajectory waypoint
struct TrajectoryPoint {
    Position3D position;
    Scalar speed{0.0};            ///< Desired speed (m/s)
    Scalar acceleration{0.0};     ///< Desired acceleration (m/s²)
    Scalar curvature{0.0};        ///< Path curvature (1/m)
    Scalar heading{0.0};          ///< Heading angle (rad)
    Duration time_offset{};       ///< Time from trajectory start
};

/// Planned trajectory
struct Trajectory {
    std::vector<TrajectoryPoint> points;
    Timestamp start_time{};
    Duration total_duration{};
    Scalar cost{0.0};              ///< Planning cost
    bool is_fallback{false};       ///< True if this is a fallback trajectory
};

// ============================================================================
// Safety Types
// ============================================================================

/// Safety severity level
enum class SafetySeverity : uint8_t {
    Info,
    Warning,
    Critical,
    Emergency
};

/// Safety event
struct SafetyEvent {
    uint64_t id{0};
    SafetySeverity severity{SafetySeverity::Info};
    std::string source;
    std::string description;
    Timestamp timestamp{};
};

} // namespace uados
