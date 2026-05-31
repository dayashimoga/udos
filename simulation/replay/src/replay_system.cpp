#include "uados/simulation/replay_system.hpp"
#include "uados/logging.hpp"

#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>

namespace uados::simulation {

UADOS_DECLARE_LOGGER("simulation.replay")

using json = nlohmann::json;

Status ReplaySystem::init(const uados::core::Config& /*config*/) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Simulation Replay System...");
    frames_.clear();

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Simulation Replay System initialized successfully.");
    return Status::Ok;
}

Status ReplaySystem::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    frames_.clear();
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status ReplaySystem::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    frames_.clear();
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void ReplaySystem::record_frame(
    double time,
    const VehicleState& ego,
    const std::vector<DetectedObject>& obstacles) noexcept {
    std::lock_guard lock(mutex_);

    if (!active_) return;

    ReplayFrame frame;
    frame.timestamp_s = time;
    frame.ego_state = ego;
    frame.obstacles = obstacles;

    frames_[time] = frame;
}

std::string ReplaySystem::serialize_log() const noexcept {
    std::lock_guard lock(mutex_);

    json log_array = json::array();

    for (const auto& [time, frame] : frames_) {
        json j_frame;
        j_frame["time_s"] = frame.timestamp_s;
        
        // Serialize ego state
        j_frame["ego_x"] = frame.ego_state.position.x;
        j_frame["ego_y"] = frame.ego_state.position.y;
        j_frame["ego_v"] = frame.ego_state.velocity.vx;
        
        double ego_yaw = frame.ego_state.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
        if (std::isnan(ego_yaw)) {
            ego_yaw = 0.0;
        }
        j_frame["ego_yaw"] = ego_yaw;

        // Serialize dynamic obstacles
        json j_obstacles = json::array();
        for (const auto& obs : frame.obstacles) {
            json j_obs;
            j_obs["id"] = obs.id;
            j_obs["class"] = static_cast<int>(obs.object_class);
            j_obs["x"] = obs.position.x;
            j_obs["y"] = obs.position.y;
            j_obs["vx"] = obs.velocity.vx;
            j_obstacles.push_back(j_obs);
        }
        j_frame["obstacles"] = j_obstacles;

        log_array.push_back(j_frame);
    }

    return log_array.dump();
}

Status ReplaySystem::load_log(const std::string& json_string) noexcept {
    std::lock_guard lock(mutex_);

    try {
        auto log_array = json::parse(json_string);
        if (!log_array.is_array()) {
            return Status::InvalidArgument;
        }

        frames_.clear();

        for (const auto& j_frame : log_array) {
            ReplayFrame frame;
            frame.timestamp_s = j_frame["time_s"].get<double>();
            
            // Reconstruct ego state
            frame.ego_state.position.x = j_frame["ego_x"].get<double>();
            frame.ego_state.position.y = j_frame["ego_y"].get<double>();
            frame.ego_state.position.z = 0.0;
            frame.ego_state.velocity.vx = j_frame["ego_v"].get<double>();
            
            double yaw = j_frame["ego_yaw"].get<double>();
            frame.ego_state.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                                          Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                                          Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ());

            // Reconstruct obstacles
            for (const auto& j_obs : j_frame["obstacles"]) {
                DetectedObject obs;
                obs.id = j_obs["id"].get<uint64_t>();
                obs.object_class = static_cast<ObjectClass>(j_obs["class"].get<int>());
                obs.position.x = j_obs["x"].get<double>();
                obs.position.y = j_obs["y"].get<double>();
                obs.position.z = 0.0;
                obs.velocity.vx = j_obs["vx"].get<double>();
                obs.velocity.vy = 0.0;
                obs.velocity.vz = 0.0;
                frame.obstacles.push_back(obs);
            }

            frames_[frame.timestamp_s] = frame;
        }

        UADOS_LOG_INFO("Replay System successfully loaded log containing {} frames.", frames_.size());
        return Status::Ok;
    } catch (const std::exception& e) {
        UADOS_LOG_ERROR("Failed to parse replay JSON log: {}", e.what());
        return Status::InvalidArgument;
    }
}

bool ReplaySystem::get_frame(double time, ReplayFrame& out_frame) const noexcept {
    std::lock_guard lock(mutex_);

    if (frames_.empty()) {
        return false;
    }

    // Find the closest time frame match (lower bound match or nearest)
    auto it = frames_.lower_bound(time);
    
    if (it == frames_.end()) {
        // past the end, return the last frame
        out_frame = frames_.rbegin()->second;
        return true;
    }

    if (it == frames_.begin()) {
        // before the beginning, return the first frame
        out_frame = it->second;
        return true;
    }

    // Compare with previous frame to find the closest match
    auto prev_it = std::prev(it);
    double diff_next = std::abs(it->first - time);
    double diff_prev = std::abs(prev_it->first - time);

    if (diff_prev < diff_next) {
        out_frame = prev_it->second;
    } else {
        out_frame = it->second;
    }

    return true;
}

void ReplaySystem::clear() noexcept {
    std::lock_guard lock(mutex_);
    frames_.clear();
}

size_t ReplaySystem::get_frame_count() const noexcept {
    std::lock_guard lock(mutex_);
    return frames_.size();
}

} // namespace uados::simulation
