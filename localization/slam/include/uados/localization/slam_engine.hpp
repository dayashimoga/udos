#pragma once

/// @file slam_engine.hpp
/// @brief Odometry dead-reckoning SLAM engine.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>

namespace uados::localization {

/// @brief Odometry dead-reckoning SLAM component.
///
/// Integrates sequential relative delta poses (speed, yaw rate) to calculate
/// dead-reckoned ego poses when absolute GNSS localization locks are degraded
/// or lost (e.g. in tunnels or multi-story parking garages).
class SLAMEngine final : public uados::core::ComponentBase {
public:
    SLAMEngine() = default;
    ~SLAMEngine() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "localization.slam"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Reset dead reckoning position
    /// @param pose The reference starting pose
    void reset_pose(const Pose& pose);

    /// Perform a dead-reckoning integration step
    /// @param linear_velocity Forward speed (m/s)
    /// @param angular_velocity Yaw rate (rad/s)
    /// @param dt Time delta (seconds)
    /// @return Integrated estimated pose
    [[nodiscard]] Pose update_odometry(double linear_velocity, double angular_velocity, double dt);

    /// Get current relative pose
    [[nodiscard]] Pose current_pose() const;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    Pose current_pose_;
    double yaw_{0.0};
};

} // namespace uados::localization
