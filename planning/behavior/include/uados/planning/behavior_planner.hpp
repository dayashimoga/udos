#pragma once

/// @file behavior_planner.hpp
/// @brief Tactical behavior state machine for maneuvers.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/localization/hdmap_engine.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::planning {

/// @brief Tactical behavior planner states
enum class BehaviorState : uint8_t {
    Cruise,                  ///< Maintain velocity matching speed limits
    StopAtStopLine,          ///< Decelerate to stop before upcoming stop lines
    IntersectionCrossing,    ///< Safely traverse intersection zones
    EmergencyStop,           ///< Execute dynamic safe fallback deceleration
    YieldObstacle            ///< Decelerate to yield or queue behind path obstacles
};

/// @brief Converts BehaviorState to string representation
[[nodiscard]] constexpr std::string_view behavior_state_to_string(BehaviorState s) noexcept {
    switch (s) {
        case BehaviorState::Cruise:               return "Cruise";
        case BehaviorState::StopAtStopLine:       return "StopAtStopLine";
        case BehaviorState::IntersectionCrossing: return "IntersectionCrossing";
        case BehaviorState::EmergencyStop:        return "EmergencyStop";
        case BehaviorState::YieldObstacle:        return "YieldObstacle";
    }
    return "Unknown";
}

/// @brief Decision structure outputted by the behavior FSM
struct BehaviorDecision {
    BehaviorState state{BehaviorState::Cruise};
    double target_speed{0.0};             ///< Desired longitudinal velocity (m/s)
    double target_acceleration{0.0};      ///< Desired longitudinal acceleration (m/s²)
    double stop_distance{-1.0};           ///< Remaining distance to stop line/obstacle (m)
    bool emergency_braking{false};        ///< Safety envelope trigger flag
};

/// @brief Tactical Behavior FSM Planner component.
///
/// Assesses environmental state (dynamic predictions, lane speed boundaries,
/// stop line landmarks, traffic light rules) and manages tactical state switches.
class BehaviorPlanner final : public uados::core::ComponentBase {
public:
    BehaviorPlanner() = default;
    ~BehaviorPlanner() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "planning.behavior"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Evaluates current state to select tactical maneuvers
    /// @param ego_pose Current position/orientation of the ego vehicle
    /// @param ego_velocity Current velocity of the ego vehicle
    /// @param route Active global route lanelets
    /// @param current_lanelet_info HD Map properties of the local ego lanelet segment
    /// @param dynamic_obstacles Monitored surrounding targets
    /// @param traffic_light_state Detected state of the nearest signal
    /// @return Active behavior state and command recommendations
    [[nodiscard]] BehaviorDecision plan(
        const Pose& ego_pose,
        const Velocity3D& ego_velocity,
        const std::vector<std::string>& route,
        const uados::localization::LaneletInfo& current_lanelet_info,
        const std::vector<DetectedObject>& dynamic_obstacles,
        TrafficLightState traffic_light_state);

private:
    mutable std::mutex mutex_;
    bool active_{false};

    BehaviorState current_state_{BehaviorState::Cruise};

    // Configuration settings
    double cruise_speed_margin_{0.0};
    double stop_distance_buffer_{2.5}; // Stop 2.5m behind stop line
};

} // namespace uados::planning
