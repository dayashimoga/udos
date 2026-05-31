#pragma once

/// @file vehicle_twin.hpp
/// @brief Kinematic bicycle dynamics vehicle digital twin simulator.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>

namespace uados::digital_twin {

/// @brief Vehicle digital twin simulator component.
///
/// Models dynamic rigid body motion using a 4-DOF Kinematic Bicycle dynamics model,
/// incorporating lateral side-slip angles (beta) and Ackermann kinematics.
class VehicleDigitalTwin final : public uados::core::ComponentBase {
public:
    VehicleDigitalTwin() = default;
    ~VehicleDigitalTwin() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "digital_twin.vehicle"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Advances the vehicle dynamics physics by a time step dt
    /// @param steer Steering angle command input (rad)
    /// @param accel Acceleration command input (m/s²)
    /// @param dt Time delta step (s)
    void step(double steer, double accel, double dt) noexcept;

    /// Resets the vehicle twin to starting poses
    /// @param pose Initial starting pose
    /// @param initial_speed Initial speed (m/s)
    void reset(const Pose& pose, double initial_speed) noexcept;

    /// Query the current simulated vehicle state
    [[nodiscard]] VehicleState get_state() const noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // State variables
    double x_{0.0};
    double y_{0.0};
    double yaw_{0.0};
    double v_{0.0};

    // Dynamic parameters
    double wheelbase_{2.7};           ///< Vehicle wheelbase L (m)
    double lr_{1.35};                 ///< CG to rear axle distance (m)
    double max_speed_{40.0};          ///< Max vehicle speed limit (m/s)
};

} // namespace uados::digital_twin
