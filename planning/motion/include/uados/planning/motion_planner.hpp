#pragma once

/// @file motion_planner.hpp
/// @brief Dynamic motion trajectory generation with obstacle avoidance.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/localization/hdmap_engine.hpp"
#include "uados/planning/behavior_planner.hpp"

#include <mutex>
#include <vector>

namespace uados::planning {

/// @brief Local Motion Planner component.
///
/// Synthesizes dynamically feasible, comfortable, collision-free local trajectories
/// by combining longitudinal constant acceleration and lateral quintic polynomials.
class MotionPlanner final : public uados::core::ComponentBase {
public:
    MotionPlanner() = default;
    ~MotionPlanner() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "planning.motion"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Generates the optimal local motion trajectory
    /// @param ego_state Current kinematic position, velocity, and acceleration
    /// @param behavior_decision Selected tactical target maneuver and speed recommendations
    /// @param current_lanelet_info Road structure properties from localization map
    /// @param dynamic_obstacles Monitored surrounding targets
    /// @param horizon_seconds Planning lookahead time horizon (s)
    /// @param step_seconds Time interval between trajectory waypoints (s)
    /// @return Complete, dynamically feasible trajectory
    [[nodiscard]] Trajectory plan_trajectory(
        const KinematicState& ego_state,
        const BehaviorDecision& behavior_decision,
        const uados::localization::LaneletInfo& current_lanelet_info,
        const std::vector<DetectedObject>& dynamic_obstacles,
        double horizon_seconds = 4.0,
        double step_seconds = 0.1) const;

    /// Checks if a planned trajectory poses a collision risk with any obstacle
    /// @param trajectory The generated candidate path
    /// @param dynamic_obstacles Active tracking targets
    /// @return True if a collision overlap is predicted
    [[nodiscard]] bool check_collision(
        const Trajectory& trajectory,
        const std::vector<DetectedObject>& dynamic_obstacles) const;

    /// Generates a safe fallback (high-deceleration to stop) trajectory in emergency scenarios
    /// @param ego_state Current kinematic position, velocity, and acceleration
    /// @param horizon_seconds Planning lookahead time horizon (s)
    /// @param step_seconds Time interval between trajectory waypoints (s)
    /// @return Fail-safe emergency decelerating path
    [[nodiscard]] Trajectory generate_fallback_trajectory(
        const KinematicState& ego_state,
        double horizon_seconds = 4.0,
        double step_seconds = 0.1) const;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // Dynamic constraints (adjustable via YAML config)
    double max_acceleration_{3.0};        ///< Max longitudinal acceleration (m/s²)
    double max_deceleration_{5.0};        ///< Comfortable longitudinal deceleration (m/s²)
    double emergency_deceleration_{8.0};  ///< Safety critical max braking (m/s²)
    double lateral_accel_limit_{2.0};     ///< Max lateral comfort acceleration (m/s²)
    double safety_collision_buffer_{1.8}; ///< Collision checking clearance buffer (m)
};

} // namespace uados::planning
