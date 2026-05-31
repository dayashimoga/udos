#pragma once

/// @file lane_detector.hpp
/// @brief Camera-based lane detection and boundary tracking.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/sensors/sensor.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::perception {

/// @brief Struct representing a lane boundary line fitted as a polynomial
/// (typically y = c3 * x^3 + c2 * x^2 + c1 * x + c0)
struct LaneBoundary {
    std::string id;                  ///< e.g., "left", "right"
    std::vector<double> coefficients; ///< cubic polynomial coefficients [c0, c1, c2, c3]
    double confidence{0.0};
    bool valid{false};
};

/// @brief Struct representing the ego lane information
struct EgoLane {
    LaneBoundary left_boundary;
    LaneBoundary right_boundary;
    double lane_width{3.5};         ///< Est width in meters
    double lateral_offset{0.0};     ///< Ego lateral offset from center (m, positive right)
    double heading_error{0.0};       ///< Yaw heading error relative to lane (rad)
};

/// @brief Lane Detector component.
class LaneDetector final : public uados::core::ComponentBase {
public:
    LaneDetector() = default;
    ~LaneDetector() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "perception.lanes"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Detect the ego lane and boundaries in the current camera frame
    /// @param frame Camera image frame
    /// @return Complete ego lane structure
    [[nodiscard]] EgoLane detect(const sensors::ImageFrame& frame);

private:
    mutable std::mutex mutex_;
    bool active_{false};
};

} // namespace uados::perception
