#pragma once

/// @file pose_estimator.hpp
/// @brief Local and global ego state pose tracking.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>

namespace uados::localization {

/// @brief Coordinates local and global ego state tracking.
///
/// Converts WGS84 geodesic coordinates (latitude, longitude, altitude)
/// into a flat, local tangent plan (e.g. UTM or ENU frame) relative
/// to a configured map origin, and publishes local 6-DOF poses.
class PoseEstimator final : public uados::core::ComponentBase {
public:
    PoseEstimator() = default;
    ~PoseEstimator() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "localization.pose"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Project geodesic coordinates to local tangent plan (ENU)
    /// @param lat Geodesic latitude in degrees
    /// @param lon Geodesic longitude in degrees
    /// @param alt Geodesic altitude in meters
    /// @param[out] x Local ENU X position in meters (East)
    /// @param[out] y Local ENU Y position in meters (North)
    void geodesic_to_local(double lat, double lon, double alt, double& x, double& y) const noexcept;

    /// Convert local ENU coordinates back to WGS84 geodesic coordinates
    /// @param x Local East coordinate (m)
    /// @param y Local North coordinate (m)
    /// @param[out] lat Geodesic latitude (deg)
    /// @param[out] lon Geodesic longitude (deg)
    void local_to_geodesic(double x, double y, double& lat, double& lon) const noexcept;

    /// Set current active pose estimate
    void update_pose(const Pose& pose);

    /// Get current pose estimate
    [[nodiscard]] Pose current_pose() const;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // Projection origin (geodesic coordinates)
    double origin_lat_{37.7749};
    double origin_lon_{-122.4194};

    Pose current_pose_;
};

} // namespace uados::localization
