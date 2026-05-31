#include "uados/prediction/risk_estimator.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::prediction {

UADOS_DECLARE_LOGGER("prediction.risk")

Status RiskEstimator::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);
    UADOS_LOG_INFO("Initializing Threat Assessment Risk Estimator...");
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);
    return Status::Ok;
}

Status RiskEstimator::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status RiskEstimator::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<ObstacleRisk> RiskEstimator::estimate_risks(
    const VehicleState& ego_state,
    const std::vector<DetectedObject>& obstacles) {

    std::lock_guard lock(mutex_);
    if (!active_) return {};

    std::vector<ObstacleRisk> risks;
    risks.reserve(obstacles.size());

    double ego_speed = ego_state.velocity.magnitude();

    for (const auto& obj : obstacles) {
        ObstacleRisk risk;
        risk.obstacle_id = obj.id;
        risk.time_to_collision = 99.9; // default no threat
        risk.threat_level = ThreatLevel::Low;

        // Longitudinal distance: obstacle position x is the forward distance in vehicle frame
        double dx = obj.position.x;
        // Lateral coordinate y (Left positive, Right negative).
        double dy = obj.position.y;

        // Path safety tunnel width: if $|dy| < 1.3$ meters, it lies directly in the path
        if (std::abs(dy) < 1.3 && dx > 0.0) {
            // Relative speed closing rate: closing_rate = ego_speed - obstacle_forward_velocity
            // (Note: if obstacle is moving away, closing rate is negative)
            // obj.velocity.vx is relative to vehicle or absolute depending on frame
            // Let's assume absolute velocities: closing rate = ego_speed - obj.velocity.vx
            double closing_rate = ego_speed - obj.velocity.vx;

            if (closing_rate > 0.1) {
                double ttc = dx / closing_rate;
                risk.time_to_collision = ttc;

                if (ttc < 1.8) {
                    risk.threat_level = ThreatLevel::Critical;
                    UADOS_LOG_WARN("CRITICAL THREAT: Obstacle ID={} ahead at {:.2f}m. TTC={:.2f}s!",
                                   obj.id, dx, ttc);
                } else if (ttc < 3.5) {
                    risk.threat_level = ThreatLevel::Warning;
                    UADOS_LOG_WARN("Threat Warning: Obstacle ID={} ahead at {:.2f}m. TTC={:.2f}s.",
                                   obj.id, dx, ttc);
                }
            }
        }

        risks.push_back(risk);
    }

    return risks;
}

} // namespace uados::prediction
