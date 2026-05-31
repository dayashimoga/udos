#include "uados/planning/behavior_planner.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::planning {

UADOS_DECLARE_LOGGER("planning.behavior")

Status BehaviorPlanner::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Behavior Decision Planner...");

    if (config) {
        if (config["stop_distance_buffer"]) {
            stop_distance_buffer_ = config["stop_distance_buffer"].as<double>();
        }
    }

    current_state_ = BehaviorState::Cruise;
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Behavior Decision Planner initialized. Stop buffer: {:.2f}m", stop_distance_buffer_);
    return Status::Ok;
}

Status BehaviorPlanner::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    current_state_ = BehaviorState::Cruise;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status BehaviorPlanner::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

BehaviorDecision BehaviorPlanner::plan(
    const Pose& ego_pose,
    const Velocity3D& ego_velocity,
    const std::vector<std::string>& route,
    const uados::localization::LaneletInfo& current_lanelet_info,
    const std::vector<DetectedObject>& dynamic_obstacles,
    TrafficLightState traffic_light_state) {
    std::lock_guard lock(mutex_);

    if (!active_) {
        UADOS_LOG_WARN("BehaviorPlanner: Plan requested while inactive.");
        return {};
    }

    double ego_speed = ego_velocity.magnitude();
    double ego_x = ego_pose.position.x;
    double ego_y = ego_pose.position.y;

    BehaviorDecision decision;
    decision.state = current_state_;
    decision.target_speed = current_lanelet_info.speed_limit_mps;
    decision.target_acceleration = 1.0; // default comfortable acceleration
    decision.stop_distance = -1.0;
    decision.emergency_braking = false;

    // 1. Evaluate Dynamic Obstacles for Safety Threats (Highest Priority)
    const DetectedObject* primary_threat = nullptr;
    double min_threat_distance = 999.0;
    double lateral_safety_tunnel = 1.3; // meters left/right of centerline

    for (const auto& obs : dynamic_obstacles) {
        // Evaluate obstacles in our path corridor (y ~= centerline_y = 0 in mock ENU)
        double dx = obs.position.x - ego_x;
        double dy = obs.position.y; // centerline is y=0

        if (dx > 0.0 && dx < 20.0 && std::abs(dy) < lateral_safety_tunnel) {
            if (dx < min_threat_distance) {
                min_threat_distance = dx;
                primary_threat = &obs;
            }
        }
    }

    if (primary_threat != nullptr) {
        double ttc = 99.0;
        double speed_diff = ego_speed - primary_threat->velocity.magnitude();
        if (speed_diff > 0.1) {
            ttc = min_threat_distance / speed_diff;
        }

        UADOS_LOG_DEBUG("Obstacle threat ahead: ID={}, dist={:.2f}m, ttc={:.2f}s",
                        primary_threat->id, min_threat_distance, ttc);

        if (min_threat_distance < 4.0 || ttc < 1.0) {
            // Extreme dynamic danger -> Emergency Fallback Stop
            if (current_state_ != BehaviorState::EmergencyStop) {
                UADOS_LOG_WARN("FSM Transition: {} -> EmergencyStop (Threat ID={} at {:.2f}m)",
                               behavior_state_to_string(current_state_), primary_threat->id, min_threat_distance);
                current_state_ = BehaviorState::EmergencyStop;
            }
            decision.state = BehaviorState::EmergencyStop;
            decision.target_speed = 0.0;
            decision.target_acceleration = -5.0; // full braking deceleration
            decision.stop_distance = min_threat_distance;
            decision.emergency_braking = true;
            return decision;
        } else if (min_threat_distance < 10.0 || ttc < 3.0) {
            // Adaptive yielding maneuver
            if (current_state_ != BehaviorState::YieldObstacle) {
                UADOS_LOG_INFO("FSM Transition: {} -> YieldObstacle (Threat ID={} at {:.2f}m)",
                               behavior_state_to_string(current_state_), primary_threat->id, min_threat_distance);
                current_state_ = BehaviorState::YieldObstacle;
            }
            decision.state = BehaviorState::YieldObstacle;
            
            // Slow down smoothly behind the obstacle
            double safe_speed = primary_threat->velocity.magnitude();
            if (min_threat_distance < 6.0) {
                safe_speed = std::max(0.0, safe_speed - 2.0); // slow down below obstacle speed
            }
            decision.target_speed = safe_speed;
            decision.target_acceleration = -1.5;
            decision.stop_distance = min_threat_distance;
            return decision;
        }
    }

    // 2. Evaluate Road Rules, Landmarks, and Traffic Lights
    bool approaching_stop_line = current_lanelet_info.has_stop_line && 
                                 current_lanelet_info.stop_line_distance > 0.0 &&
                                 current_lanelet_info.stop_line_distance < 15.0;

    bool signal_requires_stop = traffic_light_state == TrafficLightState::Red ||
                                 traffic_light_state == TrafficLightState::Yellow;

    switch (current_state_) {
        case BehaviorState::Cruise:
            if (approaching_stop_line && signal_requires_stop) {
                UADOS_LOG_INFO("FSM Transition: Cruise -> StopAtStopLine (Red/Yellow Light, dist={:.1f}m)",
                               current_lanelet_info.stop_line_distance);
                current_state_ = BehaviorState::StopAtStopLine;
                decision.state = BehaviorState::StopAtStopLine;
                decision.target_speed = 0.0;
                decision.stop_distance = current_lanelet_info.stop_line_distance;
            } else if (current_lanelet_info.is_intersection) {
                current_state_ = BehaviorState::IntersectionCrossing;
                decision.state = BehaviorState::IntersectionCrossing;
                decision.target_speed = std::min(decision.target_speed, current_lanelet_info.speed_limit_mps);
            }
            break;

        case BehaviorState::StopAtStopLine:
            if (!signal_requires_stop || !approaching_stop_line) {
                UADOS_LOG_INFO("FSM Transition: StopAtStopLine -> IntersectionCrossing (Light is Green / Passed line)");
                current_state_ = BehaviorState::IntersectionCrossing;
                decision.state = BehaviorState::IntersectionCrossing;
            } else {
                decision.target_speed = 0.0;
                decision.target_acceleration = -2.0;
                decision.stop_distance = current_lanelet_info.stop_line_distance;
            }
            break;

        case BehaviorState::IntersectionCrossing:
            if (!current_lanelet_info.is_intersection) {
                UADOS_LOG_INFO("FSM Transition: IntersectionCrossing -> Cruise (Exited intersection)");
                current_state_ = BehaviorState::Cruise;
                decision.state = BehaviorState::Cruise;
            } else {
                decision.target_speed = std::min(decision.target_speed, current_lanelet_info.speed_limit_mps);
            }
            break;

        case BehaviorState::YieldObstacle:
            // Recovery: obstacle has cleared the tunnel
            UADOS_LOG_INFO("FSM Transition: YieldObstacle -> Cruise (Path is clear)");
            current_state_ = BehaviorState::Cruise;
            decision.state = BehaviorState::Cruise;
            break;

        case BehaviorState::EmergencyStop:
            // Recovery: obstacle has cleared the path
            UADOS_LOG_INFO("FSM Transition: EmergencyStop -> Cruise (Path is clear, resuming)");
            current_state_ = BehaviorState::Cruise;
            decision.state = BehaviorState::Cruise;
            break;
    }

    return decision;
}

} // namespace uados::planning
