#include "uados/localization/hdmap_engine.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::localization {

UADOS_DECLARE_LOGGER("localization.hdmap")

Status HDMapEngine::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing HD Map Engine...");

    load_mock_map();

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("HD Map Engine initialized: loaded {} lanelet segments", lanelets_.size());
    return Status::Ok;
}

Status HDMapEngine::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status HDMapEngine::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

LaneletInfo HDMapEngine::get_nearest_lanelet(const Pose& pose) const {
    std::lock_guard lock(mutex_);
    if (!active_ || lanelets_.empty()) return {};

    double px = pose.position.x;
    double py = pose.position.y;

    // Find closest lanelet by longitudinal segment overlap
    MapLanelet closest = lanelets_[0];
    double min_dist = std::numeric_limits<double>::max();
    bool found = false;

    for (const auto& l : lanelets_) {
        if (px >= l.start_x && px <= l.end_x) {
            closest = l;
            found = true;
            break;
        }
        
        // Fallback closest by distance to center
        double dist = std::abs(py - l.center_y);
        if (dist < min_dist) {
            min_dist = dist;
            closest = l;
        }
    }

    LaneletInfo info;
    info.id = closest.id;
    info.speed_limit_mps = closest.speed_limit;
    info.centerline_heading = closest.heading;
    
    // Calculate distance to centerline
    // centerline is horizontal y = center_y for straight lanes
    info.distance_to_centerline = py - closest.center_y;
    info.is_intersection = closest.intersection;
    info.has_stop_line = closest.stop_line;

    if (closest.stop_line && closest.stop_line_x >= px) {
        info.stop_line_distance = closest.stop_line_x - px;
    } else {
        info.stop_line_distance = -1.0;
    }

    UADOS_LOG_DEBUG("HDMap Query: pose=({:.1f}, {:.1f}) -> lanelet='{}', distance_to_centerline={:.2f}m, stop_line_dist={:.1f}m",
                    px, py, info.id, info.distance_to_centerline, info.stop_line_distance);

    return info;
}

void HDMapEngine::load_mock_map() {
    lanelets_.clear();

    // segment 1: Straight road, speed limit 50 km/h (13.8 m/s)
    MapLanelet l1;
    l1.id = "lanelet_1001";
    l1.start_x = 0.0;
    l1.end_x = 40.0;
    l1.center_y = 0.0;
    l1.heading = 0.0; // East
    l1.speed_limit = 13.8;
    l1.intersection = false;
    l1.stop_line = false;
    lanelets_.push_back(l1);

    // segment 2: Approaching intersection, speed limit 30 km/h (8.3 m/s)
    MapLanelet l2;
    l2.id = "lanelet_1002";
    l2.start_x = 40.0;
    l2.end_x = 60.0;
    l2.center_y = 0.0;
    l2.heading = 0.0;
    l2.speed_limit = 8.33;
    l2.intersection = false;
    l2.stop_line = true;
    l2.stop_line_x = 58.0; // stop line at 58 meters
    lanelets_.push_back(l2);

    // segment 3: Intersection crossing
    MapLanelet l3;
    l3.id = "lanelet_1003";
    l3.start_x = 60.0;
    l3.end_x = 80.0;
    l3.center_y = 0.0;
    l3.heading = 0.0;
    l3.speed_limit = 5.5; // slow crossing
    l3.intersection = true;
    l3.stop_line = false;
    lanelets_.push_back(l3);

    // segment 4: Exit straight lane
    MapLanelet l4;
    l4.id = "lanelet_1004";
    l4.start_x = 80.0;
    l4.end_x = 200.0;
    l4.center_y = 0.0;
    l4.heading = 0.0;
    l4.speed_limit = 13.8;
    l4.intersection = false;
    l4.stop_line = false;
    lanelets_.push_back(l4);
}

} // namespace uados::localization
