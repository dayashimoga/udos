#pragma once

/// @file replay_system.hpp
/// @brief Frame recording and JSON replay log serialization.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>
#include <map>

namespace uados::simulation {

/// @brief Struct representing a saved time frame log
struct ReplayFrame {
    double timestamp_s{0.0};
    VehicleState ego_state;
    std::vector<DetectedObject> obstacles;
};

/// @brief Replay System component.
///
/// Records historical simulation frames and serializes them frame-accurately
/// to JSON streams, enabling offline replay and debugging.
class ReplaySystem final : public uados::core::ComponentBase {
public:
    ReplaySystem() = default;
    ~ReplaySystem() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "simulation.replay"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Caches a single time step frame log
    /// @param time Time step offset (s)
    /// @param ego Current ego vehicle state
    /// @param obstacles Captured environmental dynamic targets
    void record_frame(
        double time,
        const VehicleState& ego,
        const std::vector<DetectedObject>& obstacles) noexcept;

    /// Serializes cached time frames to JSON string
    [[nodiscard]] std::string serialize_log() const noexcept;

    /// Clears and deserializes frame log from JSON string
    /// @param json_string Serialized log data
    /// @return Status::Ok on success
    [[nodiscard]] Status load_log(const std::string& json_string) noexcept;

    /// Queries the logged frame matching or nearest to a specific time step
    /// @param time Target time step offset (s)
    /// @param out_frame Output parameter to write queried frame
    /// @return True if a matching frame was found
    [[nodiscard]] bool get_frame(double time, ReplayFrame& out_frame) const noexcept;

    /// Clear all recorded frames
    void clear() noexcept;

    /// Query total count of recorded frames
    [[nodiscard]] size_t get_frame_count() const noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    std::map<double, ReplayFrame> frames_;
};

} // namespace uados::simulation
