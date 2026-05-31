#include "uados/simulation/scenario_engine.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::simulation {

UADOS_DECLARE_LOGGER("simulation.scenarios")

Status ScenarioEngine::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Scenario Engine...");

    ASSERT_EQ(vehicle_twin_.init(config), Status::Ok);
    ASSERT_EQ(sensor_twin_.init(config), Status::Ok);

    metrics_ = ScenarioMetrics();
    traffic_agents_.clear();

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Scenario Engine initialized successfully.");
    return Status::Ok;
}

Status ScenarioEngine::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    ASSERT_EQ(vehicle_twin_.start(), Status::Ok);
    ASSERT_EQ(sensor_twin_.start(), Status::Ok);

    metrics_ = ScenarioMetrics();
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status ScenarioEngine::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    EXPECT_EQ(vehicle_twin_.stop(), Status::Ok);
    EXPECT_EQ(sensor_twin_.stop(), Status::Ok);

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void ScenarioEngine::load_scenario(
    double start_x,
    double start_y,
    double start_speed,
    const std::vector<DetectedObject>& traffic_agents) noexcept {
    std::lock_guard lock(mutex_);

    Pose initial_pose;
    initial_pose.position = {start_x, start_y, 0.0};
    initial_pose.orientation = Quat::Identity();

    vehicle_twin_.reset(initial_pose, start_speed);
    traffic_agents_ = traffic_agents;

    // Reset metrics
    metrics_ = ScenarioMetrics();
    metrics_.min_obstacle_distance = 999.0;
    metrics_.total_simulated_time = 0.0;

    UADOS_LOG_INFO("Scenario loaded: {} dynamic traffic obstacles, start position=({:.1f}, {:.1f}), speed={:.1f}m/s",
                   traffic_agents_.size(), start_x, start_y, start_speed);
}

void ScenarioEngine::step(double steer, double accel, double dt) noexcept {
    std::lock_guard lock(mutex_);

    if (!active_ || dt <= 0.0001) {
        return;
    }

    // 1. Advance Ego Vehicle Digital Twin dynamics
    vehicle_twin_.step(steer, accel, dt);

    // 2. Linearly propagate surrounding traffic agent locations
    for (auto& agent : traffic_agents_) {
        agent.position.x += agent.velocity.vx * dt;
        agent.position.y += agent.velocity.vy * dt;
    }

    // 3. Update active scenario metrics
    auto ego_state = vehicle_twin_.get_state();
    double ego_x = ego_state.position.x;
    double ego_y = ego_state.position.y;

    metrics_.total_simulated_time += dt;

    for (const auto& agent : traffic_agents_) {
        double dx = ego_x - agent.position.x;
        double dy = ego_y - agent.position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        metrics_.min_obstacle_distance = std::min(metrics_.min_obstacle_distance, distance);

        // Check for boundary collisions
        if (distance < safety_clearance_limit_) {
            if (!metrics_.collision_occurred) {
                UADOS_LOG_ERROR("SIMULATION CRITICAL COLLISION: Ego hit traffic agent ID={} at relative distance={:.2f}m",
                                agent.id, distance);
                metrics_.collision_occurred = true;
                metrics_.safety_violations++;
            }
        }
    }
}

ScenarioMetrics ScenarioEngine::get_metrics() const noexcept {
    std::lock_guard lock(mutex_);
    return metrics_;
}

std::vector<DetectedObject> ScenarioEngine::get_traffic_agents() const noexcept {
    std::lock_guard lock(mutex_);
    return traffic_agents_;
}

} // namespace uados::simulation
