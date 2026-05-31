#pragma once

/// @file strategic_planner.hpp
/// @brief Global route planning on road graphs.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

namespace uados::planning {

/// @brief Strategic global route planner component.
///
/// Builds a graph representation of the pre-mapped HD map lanelet connections
/// and searches for the optimal sequence of lanelets to reach a target segment.
class StrategicPlanner final : public uados::core::ComponentBase {
public:
    StrategicPlanner() = default;
    ~StrategicPlanner() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "planning.strategic"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Computes a sequence of lanelet IDs from start to goal
    /// @param start_lanelet_id The starting lanelet ID
    /// @param goal_lanelet_id The destination lanelet ID
    /// @return Ordered sequence of lanelet IDs representing the global path
    [[nodiscard]] std::vector<std::string> compute_route(
        const std::string& start_lanelet_id, const std::string& goal_lanelet_id) const;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    // Simple routing graph mapping lanelet ID to its adjacent exit connections
    std::unordered_map<std::string, std::vector<std::string>> adjacency_graph_;

    void build_routing_graph();
};

} // namespace uados::planning
