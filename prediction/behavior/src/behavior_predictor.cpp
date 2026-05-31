#include "uados/prediction/behavior_predictor.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::prediction {

UADOS_DECLARE_LOGGER("prediction.behavior")

Status BehaviorPredictor::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);
    UADOS_LOG_INFO("Initializing Behavior Intention Predictor...");
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);
    return Status::Ok;
}

Status BehaviorPredictor::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status BehaviorPredictor::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<ObstacleBehavior> BehaviorPredictor::predict_intentions(
    const std::vector<DetectedObject>& tracked_obstacles) {

    std::lock_guard lock(mutex_);
    if (!active_) return {};

    std::vector<ObstacleBehavior> behaviors;
    behaviors.reserve(tracked_obstacles.size());

    for (const auto& obj : tracked_obstacles) {
        ObstacleBehavior behavior;
        behavior.obstacle_id = obj.id;

        // Classify intention based on lateral velocity and acceleration metrics
        // positive y is Left in vehicle frame
        double lateral_velocity = obj.velocity.vy;
        double longitudinal_acceleration = obj.acceleration.ax; 

        Intention primary = Intention::Cruising;
        double cruise_prob = 0.8;
        double left_prob = 0.05;
        double right_prob = 0.05;
        double braking_prob = 0.1;

        if (lateral_velocity > 0.35) {
            primary = Intention::LaneChangingLeft;
            left_prob = 0.85;
            cruise_prob = 0.1;
            right_prob = 0.01;
            braking_prob = 0.04;
        } else if (lateral_velocity < -0.35) {
            primary = Intention::LaneChangingRight;
            right_prob = 0.85;
            cruise_prob = 0.1;
            left_prob = 0.01;
            braking_prob = 0.04;
        } else if (longitudinal_acceleration < -0.8) {
            primary = Intention::Braking;
            braking_prob = 0.80;
            cruise_prob = 0.15;
            left_prob = 0.025;
            right_prob = 0.025;
        }

        behavior.primary_intention = primary;

        behavior.hypotheses.push_back({Intention::Cruising, cruise_prob});
        behavior.hypotheses.push_back({Intention::Braking, braking_prob});
        behavior.hypotheses.push_back({Intention::LaneChangingLeft, left_prob});
        behavior.hypotheses.push_back({Intention::LaneChangingRight, right_prob});

        // Sort hypotheses by probability descending
        std::sort(behavior.hypotheses.begin(), behavior.hypotheses.end(),
                  [](const IntentionHypothesis& a, const IntentionHypothesis& b) {
                      return a.probability > b.probability;
                  });

        behaviors.push_back(behavior);

        UADOS_LOG_DEBUG("Obstacle Intent: id={}, primary='{}' (conf={:.2f})",
                        obj.id, intention_to_string(primary), behavior.hypotheses[0].probability);
    }

    return behaviors;
}

} // namespace uados::prediction
