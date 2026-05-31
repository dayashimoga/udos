#pragma once

/// @file risk_estimator.hpp
/// @brief Threat assessment and Time-to-Collision (TTC) risk estimation.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::prediction {

/// @brief Threat threat levels for safety overrides
enum class ThreatLevel : uint8_t {
    Low,
    Warning,
    Critical
};

/// @brief Struct representing the assessed risk for a single obstacle
struct ObstacleRisk {
    uint64_t obstacle_id{0};
    double time_to_collision{99.9}; ///< in seconds (99.9 if no collision trajectory)
    ThreatLevel threat_level{ThreatLevel::Low};
};

/// @brief Risk Estimator component.
class RiskEstimator final : public uados::core::ComponentBase {
public:
    RiskEstimator() = default;
    ~RiskEstimator() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "prediction.risk"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Calculate the Time-to-Collision (TTC) threat indexes relative to the ego state
    /// @param ego_state Fused current ego vehicle telemetry
    /// @param obstacles Current persistent obstacles
    /// @return Assessed threat level indexes for all obstacles
    [[nodiscard]] std::vector<ObstacleRisk> estimate_risks(
        const VehicleState& ego_state,
        const std::vector<DetectedObject>& obstacles);

private:
    mutable std::mutex mutex_;
    bool active_{false};
};

} // namespace uados::prediction
