#pragma once

/// @file object_detector.hpp
/// @brief 3D bounding-box object detector.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/sensors/sensor.hpp"
#include "uados/perception/inference_engine.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::perception {

/// @brief 3D Object Detector component.
///
/// Subscribes to camera frames, runs DNN inference via InferenceEngine,
/// and projects 2D image pixel boxes into 3D world space coordinates
/// based on pinhole camera intrinsics and geometric size priors.
class ObjectDetector final : public uados::core::ComponentBase {
public:
    ObjectDetector() = default;
    ~ObjectDetector() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "perception.detection"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Detect obstacles in the given image frame and project to 3D world frame
    /// @param frame Input image frame from camera sensor
    /// @return List of detected obstacles in 3D coordinate frame
    [[nodiscard]] std::vector<DetectedObject> detect(const sensors::ImageFrame& frame);

private:
    mutable std::mutex mutex_;
    bool active_{false};

    InferenceEngine engine_;

    // Camera Pinhole Intrinsics (focal length and center pixels)
    double fx_{500.0};
    double fy_{500.0};
    double cx_{320.0};
    double cy_{240.0};

    uint64_t next_object_id_{1};

    // Depth-by-Prior Size projection helper
    [[nodiscard]] double estimate_depth(ObjectClass cls, double box_width) const noexcept;
};

} // namespace uados::perception
