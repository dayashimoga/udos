#include "uados/perception/object_tracker.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::perception {

UADOS_DECLARE_LOGGER("perception.tracking")

Status ObjectTracker::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Multi-Object Tracker (MOT)...");

    if (config) {
        if (config["gating_threshold"]) {
            gating_threshold_ = config["gating_threshold"].as<double>();
        }
        if (config["max_missed_frames"]) {
            max_missed_frames_ = config["max_missed_frames"].as<uint32_t>();
        }
    }

    tracks_.clear();
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Multi-Object Tracker initialized: gating_threshold={:.2f}m", gating_threshold_);
    return Status::Ok;
}

Status ObjectTracker::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status ObjectTracker::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<DetectedObject> ObjectTracker::track(const std::vector<DetectedObject>& detections) {
    std::lock_guard lock(mutex_);
    if (!active_) return {};

    auto now = Clock::now();
    std::vector<DetectedObject> tracked_objects;
    tracked_objects.reserve(detections.size());

    // Flag tracks that haven't been associated yet
    for (auto& [_, track] : tracks_) {
        track.missed_frames++;
    }

    // Gated Nearest Neighbor Data Association
    for (const auto& det : detections) {
        uint64_t best_track_id = 0;
        double best_dist = gating_threshold_;

        for (const auto& [id, track] : tracks_) {
            // Check matching class first
            if (track.object.object_class != det.object_class) continue;

            // Euclidean distance
            double dx = det.position.x - track.object.position.x;
            double dy = det.position.y - track.object.position.y;
            double dz = det.position.z - track.object.position.z;
            double dist = std::sqrt(dx * dx + dy * dy + dz * dz);

            if (dist < best_dist) {
                best_dist = dist;
                best_track_id = id;
            }
        }

        if (best_track_id != 0) {
            // 1. Associated! Update existing track
            auto& track = tracks_[best_track_id];
            
            // Calculate velocity: v = dx / dt
            double dt = 0.1; // Default fallback dt
            if (track.last_update_time != Timestamp{}) {
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - track.last_update_time);
                dt = static_cast<double>(elapsed.count()) / 1e6;
            }

            if (dt > 0.0) {
                double vx = (det.position.x - track.object.position.x) / dt;
                double vy = (det.position.y - track.object.position.y) / dt;
                double vz = (det.position.z - track.object.position.z) / dt;

                // Simple exponential moving average filter for velocity smoothing
                track.object.velocity.vx = 0.7 * track.object.velocity.vx + 0.3 * vx;
                track.object.velocity.vy = 0.7 * track.object.velocity.vy + 0.3 * vy;
                track.object.velocity.vz = 0.7 * track.object.velocity.vz + 0.3 * vz;
            }

            track.object.position = det.position;
            track.object.confidence = det.confidence;
            track.object.timestamp = now;
            track.missed_frames = 0; // Reset misses
            track.active_frames++;
            track.last_update_time = now;

            tracked_objects.push_back(track.object);
        } else {
            // 2. Not associated! Initialize a new Track (Birth)
            uint64_t new_id = next_track_id_++;
            Track new_track;
            new_track.object = det;
            new_track.object.id = new_id;
            new_track.object.timestamp = now;
            new_track.missed_frames = 0;
            new_track.active_frames = 1;
            new_track.last_update_time = now;

            tracks_[new_id] = new_track;
            tracked_objects.push_back(new_track.object);

            UADOS_LOG_DEBUG("Track Birth: created persistent ID={} for class={}",
                            new_id, static_cast<int>(det.object_class));
        }
    }

    // Clean up expired tracks (Death)
    for (auto it = tracks_.begin(); it != tracks_.end();) {
        if (it->second.missed_frames > max_missed_frames_) {
            UADOS_LOG_DEBUG("Track Death: removed persistent ID={} (missed {} frames)",
                            it->first, it->second.missed_frames);
            it = tracks_.erase(it);
        } else {
            ++it;
        }
    }

    return tracked_objects;
}

} // namespace uados::perception
