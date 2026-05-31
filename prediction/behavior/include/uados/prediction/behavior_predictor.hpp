#pragma once

/// @file behavior_predictor.hpp
/// @brief Intention and behavior classification.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::prediction {

/// @brief Intention categories for surrounding vehicles
enum class Intention : uint8_t {
    Cruising,
    Braking,
    LaneChangingLeft,
    LaneChangingRight,
    TurningLeft,
    TurningRight,
    Unknown
};

/// @brief Conversion helper to retrieve human-readable intention string
[[nodiscard]] constexpr std::string_view intention_to_string(Intention intent) noexcept {
    switch (intent) {
        case Intention::Cruising:          return "Cruising";
        case Intention::Braking:           return "Braking";
        case Intention::LaneChangingLeft:  return "LaneChangingLeft";
        case Intention::LaneChangingRight: return "LaneChangingRight";
        case Intention::TurningLeft:       return "TurningLeft";
        case Intention::TurningRight:      return "TurningRight";
        case Intention::Unknown:           return "Unknown";
    }
    return "Unknown";
}

/// @brief Struct representing a behavioral hypothesis for an obstacle
struct IntentionHypothesis {
    Intention intention{Intention::Unknown};
    double probability{0.0};
};

/// @brief Struct representing the full behavior classification output
struct ObstacleBehavior {
    uint64_t obstacle_id{0};
    Intention primary_intention{Intention::Unknown};
    std::vector<IntentionHypothesis> hypotheses;
};

/// @brief Behavior Predictor component.
class BehaviorPredictor final : public uados::core::ComponentBase {
public:
    BehaviorPredictor() = default;
    ~BehaviorPredictor() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "prediction.behavior"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Predict the behavioral intentions for a list of tracked obstacles
    /// @param tracked_obstacles Obstacles list from tracking layer
    /// @return Classified intentions and probability distributions
    [[nodiscard]] std::vector<ObstacleBehavior> predict_intentions(
        const std::vector<DetectedObject>& tracked_obstacles);

private:
    mutable std::mutex mutex_;
    bool active_{false};
};

} // namespace uados::prediction
