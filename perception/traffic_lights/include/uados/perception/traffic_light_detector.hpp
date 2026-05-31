#pragma once

/// @file traffic_light_detector.hpp
/// @brief Traffic light detection and classification.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/sensors/sensor.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::perception {

/// @brief Struct representing a detected traffic light
struct TrafficLightResult {
    uint64_t id{0};
    TrafficLightState state{TrafficLightState::Unknown};
    double confidence{0.0};
    Position3D position;            ///< 3D position in vehicle frame
};

/// @brief Traffic Light Detector component.
class TrafficLightDetector final : public uados::core::ComponentBase {
public:
    TrafficLightDetector() = default;
    ~TrafficLightDetector() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "perception.traffic_lights"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Detect traffic lights in the camera frame
    /// @param frame Camera image frame
    /// @return Active list of detected traffic lights and their states
    [[nodiscard]] std::vector<TrafficLightResult> detect(const sensors::ImageFrame& frame);

private:
    mutable std::mutex mutex_;
    bool active_{false};
    uint64_t seq_{0};
};

} // namespace uados::perception
