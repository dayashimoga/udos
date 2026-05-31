#pragma once

/// @file hdmap_engine.hpp
/// @brief HD Map coordinate querying and road rules manager.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::localization {

/// @brief Struct representing structured HD Map lanelet properties
struct LaneletInfo {
    std::string id;
    double speed_limit_mps{13.8}; // default 50 km/h
    double centerline_heading{0.0};
    double distance_to_centerline{0.0};
    bool is_intersection{false};
    bool has_stop_line{false};
    double stop_line_distance{-1.0}; // negative if no stop line ahead
};

/// @brief HD Map Engine component.
///
/// Under hardware configurations, interfaces with Lanelet2 map frameworks to load
/// OSM database files. Under virtual/simulation, queries pre-mapped road lanes.
class HDMapEngine final : public uados::core::ComponentBase {
public:
    HDMapEngine() = default;
    ~HDMapEngine() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "localization.hdmap"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Query the HD Map for the lanelet properties surrounding the ego pose
    /// @param pose The current ego pose in local map coordinates (ENU)
    /// @return Active lanelet details and traffic rules
    [[nodiscard]] LaneletInfo get_nearest_lanelet(const Pose& pose) const;

private:
    struct MapLanelet {
        std::string id;
        double start_x{0.0};
        double end_x{100.0};
        double center_y{0.0}; // Center of the lane (lateral coordinate)
        double heading{0.0};  // Centerline direction (rad)
        double speed_limit{13.8};
        bool intersection{false};
        bool stop_line{false};
        double stop_line_x{-1.0};
    };

    mutable std::mutex mutex_;
    bool active_{false};

    std::vector<MapLanelet> lanelets_;
    void load_mock_map();
};

} // namespace uados::localization
