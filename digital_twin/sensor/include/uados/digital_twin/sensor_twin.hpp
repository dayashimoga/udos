#pragma once

/// @file sensor_twin.hpp
/// @brief Pinhole camera projection and noisy radar sweeps sensor digital twin.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>
#include <random>

namespace uados::digital_twin {

/// @brief Struct representing a 2D pixel coordinate point
struct PixelPoint {
    double u{0.0};
    double v{0.0};
    bool visible{false};
};

/// @brief Sensor digital twin simulator component.
///
/// Simulates synthetic sensor sweeps, performing coordinate pinhole camera projections
/// and simulating noisy radar targets with configured covariance noise distributions.
class SensorDigitalTwin final : public uados::core::ComponentBase {
public:
    SensorDigitalTwin() = default;
    ~SensorDigitalTwin() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "digital_twin.sensor"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Projects 3D coordinate point in vehicle space into 2D camera pixel coordinate space
    /// @param point_3d Point in vehicle space (m) (x-forward, y-left, z-up)
    /// @param camera_pose Position/orientation of the camera sensor in vehicle space
    /// @return projected pixel coordinate
    [[nodiscard]] PixelPoint project_to_camera(const Position3D& point_3d, const Pose& camera_pose) const noexcept;

    /// Simulates noisy radar scans of static/dynamic traffic agents
    /// @param ego_state Current kinematic position of the ego vehicle
    /// @param traffic_agents Surrounding simulated traffic obstacles
    /// @return List of noisy radar detected dynamic obstacles
    [[nodiscard]] std::vector<DetectedObject> simulate_radar(
        const VehicleState& ego_state,
        const std::vector<DetectedObject>& traffic_agents) noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // Camera Intrinsics
    double fx_{1200.0};                ///< Focal length horizontal
    double fy_{1200.0};                ///< Focal length vertical
    double cx_{960.0};                 ///< Center principal point horizontal
    double cy_{540.0};                 ///< Center principal point vertical
    int width_{1920};
    int height_{1080};

    // Radar sensor characteristics
    double radar_range_limit_{100.0};   ///< Maximum radar detection range (m)
    double radar_fov_rad_{0.785};       ///< Radar field of view (45 degrees in rad)
    double noise_range_std_{0.15};      ///< Distance noise std dev (m)
    double noise_velocity_std_{0.25};   ///< Velocity noise std dev (m/s)

    // Gaussian noise generators
    std::random_device rd_;
    std::mt19937 generator_;
};

} // namespace uados::digital_twin
