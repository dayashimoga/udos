#include "uados/planning/strategic_planner.hpp"
#include "uados/logging.hpp"

#include <queue>
#include <unordered_set>
#include <algorithm>

namespace uados::planning {

UADOS_DECLARE_LOGGER("planning.strategic")

Status StrategicPlanner::init(const uados::core::Config& /*config*/) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Strategic Routing Planner...");

    build_routing_graph();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Strategic Routing Planner initialized successfully.");
    return Status::Ok;
}

Status StrategicPlanner::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status StrategicPlanner::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<std::string> StrategicPlanner::compute_route(
    const std::string& start_lanelet_id, const std::string& goal_lanelet_id) const {
    std::lock_guard lock(mutex_);

    if (!active_) {
        UADOS_LOG_WARN("StrategicPlanner requested to compute route while inactive.");
        return {};
    }

    UADOS_LOG_INFO("Computing global route from '{}' to '{}'", start_lanelet_id, goal_lanelet_id);

    if (start_lanelet_id == goal_lanelet_id) {
        return {start_lanelet_id};
    }

    if (adjacency_graph_.find(start_lanelet_id) == adjacency_graph_.end()) {
        UADOS_LOG_ERROR("Start lanelet '{}' not found in routing graph.", start_lanelet_id);
        return {};
    }

    // Standard BFS to find shortest path in routing graph
    std::queue<std::string> q;
    std::unordered_map<std::string, std::string> parent_map;
    std::unordered_set<std::string> visited;

    q.push(start_lanelet_id);
    visited.insert(start_lanelet_id);

    bool found = false;
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();

        if (current == goal_lanelet_id) {
            found = true;
            break;
        }

        auto it = adjacency_graph_.find(current);
        if (it != adjacency_graph_.end()) {
            for (const auto& neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    parent_map[neighbor] = current;
                    q.push(neighbor);
                }
            }
        }
    }

    if (!found) {
        UADOS_LOG_ERROR("No viable route found from '{}' to '{}'", start_lanelet_id, goal_lanelet_id);
        return {};
    }

    // Reconstruct route path
    std::vector<std::string> route;
    std::string curr = goal_lanelet_id;
    while (curr != start_lanelet_id) {
        route.push_back(curr);
        curr = parent_map[curr];
    }
    route.push_back(start_lanelet_id);
    std::reverse(route.begin(), route.end());

    UADOS_LOG_INFO("Route successfully generated: {} hops", route.size());
    return route;
}

void StrategicPlanner::build_routing_graph() {
    adjacency_graph_.clear();

    // Map graph topology for mock maps (aligns with HDMapEngine)
    // lanelet_1001 (Straight) -> lanelet_1002 (Approach Stop Line)
    adjacency_graph_["lanelet_1001"] = {"lanelet_1002"};

    // lanelet_1002 (Stop Line) -> lanelet_1003 (Intersection Crossing)
    adjacency_graph_["lanelet_1002"] = {"lanelet_1003"};

    // lanelet_1003 (Intersection Crossing) -> lanelet_1004 (Exit Straight)
    adjacency_graph_["lanelet_1003"] = {"lanelet_1004"};

    // lanelet_1004 (Exit Straight) is terminal in our mock layout
    adjacency_graph_["lanelet_1004"] = {};
}

} // namespace uados::planning
