#pragma once

/// @file trajectory_predictor.hpp
/// @brief Future path trajectory prediction.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::prediction {

/// @brief Struct representing a predicted future path for an obstacle
struct PredictedPath {
    std::vector<TrajectoryPoint> points;
    double probability{1.0};
};

/// @brief Struct representing the full prediction output for an obstacle
struct ObstaclePrediction {
    uint64_t obstacle_id{0};
    ObjectClass object_class{ObjectClass::Unknown};
    std::vector<PredictedPath> paths;
};

/// @brief Trajectory Predictor component.
///
/// Implements kinematic Constant Velocity (CV) and Constant Acceleration (CA)
/// motion models to project obstacle positions over a future time horizon.
class TrajectoryPredictor final : public uados::core::ComponentBase {
public:
    TrajectoryPredictor() = default;
    ~TrajectoryPredictor() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "prediction.trajectory"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Predict future paths for a list of tracked objects
    /// @param tracked_obstacles Persistent obstacles from tracker
    /// @param horizon_seconds Planning lookahead time (seconds)
    /// @param step_seconds Interval between trajectory points (seconds)
    /// @return Future trajectory predictions for all obstacles
    [[nodiscard]] std::vector<ObstaclePrediction> predict(
        const std::vector<DetectedObject>& tracked_obstacles,
        double horizon_seconds = 5.0,
        double step_seconds = 0.5);

private:
    mutable std::mutex mutex_;
    bool active_{false};
};

} // namespace uados::prediction
