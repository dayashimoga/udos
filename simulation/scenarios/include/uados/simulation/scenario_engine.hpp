#pragma once

/// @file scenario_engine.hpp
/// @brief Batch scenario execution and dynamic test orchestrator.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/digital_twin/vehicle_twin.hpp"
#include "uados/digital_twin/sensor_twin.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::simulation {

/// @brief Safety metrics caught during scenario execution
struct ScenarioMetrics {
    bool collision_occurred{false};
    double min_obstacle_distance{999.0};
    double max_cross_track_error{0.0};
    double max_speed_error{0.0};
    double total_simulated_time{0.0};
    int speed_limit_breaches{0};
    int safety_violations{0};
};

/// @brief Scenario Engine component.
///
/// Loads virtual scenario parameters, orchestrates loop updates stepping ego dynamics,
/// propagating traffic agent coordinates, and logging safety metric deviations.
class ScenarioEngine final : public uados::core::ComponentBase {
public:
    ScenarioEngine() = default;
    ~ScenarioEngine() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "simulation.scenarios"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Loads scenario parameters from configured YAML settings
    /// @param start_x Starting ENU position x (m)
    /// @param start_y Starting ENU position y (m)
    /// @param start_speed Starting speed (m/s)
    /// @param traffic_agents List of dynamic surrounding target coordinates
    void load_scenario(
        double start_x,
        double start_y,
        double start_speed,
        const std::vector<DetectedObject>& traffic_agents) noexcept;

    /// Advances the simulation loop by a time step dt
    /// @param steer Steering actuator input (rad)
    /// @param accel Acceleration actuator input (m/s²)
    /// @param dt Time delta (s)
    void step(double steer, double accel, double dt) noexcept;

    /// Returns safety metrics collected during scenario runs
    [[nodiscard]] ScenarioMetrics get_metrics() const noexcept;

    /// Query the current simulated traffic agents
    [[nodiscard]] std::vector<DetectedObject> get_traffic_agents() const noexcept;

    /// Query the vehicle digital twin instance
    [[nodiscard]] const uados::digital_twin::VehicleDigitalTwin& get_vehicle_twin() const noexcept { return vehicle_twin_; }
    [[nodiscard]] uados::digital_twin::VehicleDigitalTwin& get_vehicle_twin() noexcept { return vehicle_twin_; }

private:
    mutable std::mutex mutex_;
    bool active_{false};

    uados::digital_twin::VehicleDigitalTwin vehicle_twin_;
    uados::digital_twin::SensorTwin sensor_twin_;

    std::vector<DetectedObject> traffic_agents_;
    ScenarioMetrics metrics_;

    double safety_clearance_limit_{1.8}; ///< Collision distance buffer (m)
};

} // namespace uados::simulation
