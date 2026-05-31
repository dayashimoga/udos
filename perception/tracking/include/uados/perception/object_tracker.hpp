#pragma once

/// @file object_tracker.hpp
/// @brief Multi-Object Tracking (MOT) system.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace uados::perception {

/// @brief Multi-Object Tracker (MOT) component.
///
/// Implements a Gated Nearest Neighbor data association tracker to match
/// real-time object detections across consecutive camera frames.
/// Automatically handles track birth, death, and estimates velocity vectors.
class ObjectTracker final : public uados::core::ComponentBase {
public:
    struct Track {
        DetectedObject object;
        uint32_t missed_frames{0};
        uint32_t active_frames{0};
        Timestamp last_update_time{};
    };

    ObjectTracker() = default;
    ~ObjectTracker() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "perception.tracking"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Track a list of raw detections and update persistence IDs
    /// @param detections Raw detections in the current frame
    /// @return Active persistent tracked objects with estimated velocities
    [[nodiscard]] std::vector<DetectedObject> track(const std::vector<DetectedObject>& detections);

private:
    mutable std::mutex mutex_;
    bool active_{false};

    double gating_threshold_{2.0}; // Max distance for association (meters)
    uint32_t max_missed_frames_{5}; // Frames before deletion

    std::unordered_map<uint64_t, Track> tracks_;
    uint64_t next_track_id_{100}; // Persistent track IDs start at 100
};

} // namespace uados::perception
