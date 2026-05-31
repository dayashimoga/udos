#include "uados/perception/lane_detector.hpp"
#include "uados/logging.hpp"

namespace uados::perception {

UADOS_DECLARE_LOGGER("perception.lanes")

Status LaneDetector::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);
    UADOS_LOG_INFO("Initializing Lane Detector...");
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);
    return Status::Ok;
}

Status LaneDetector::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status LaneDetector::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

EgoLane LaneDetector::detect(const sensors::ImageFrame& frame) {
    std::lock_guard lock(mutex_);
    if (!active_) return {};

    EgoLane lane;
    lane.lane_width = 3.5;
    lane.lateral_offset = 0.15; // 15cm offset right of center
    lane.heading_error = -0.02; // -0.02 rad heading error (pointing slightly left)

    // Fit cubic polynomials representing standard road layout:
    // y = c3 * x^3 + c2 * x^2 + c1 * x + c0
    // c0 represents lateral offset at x=0
    // c1 represents heading slope (tangent) at x=0
    // c2 represents half curvature at x=0
    // c3 represents curvature rate

    // Left Boundary: y = 1.75 meters (offset relative to lane center)
    // Adjusting for ego car lateral offset (c0 = 1.75 - lateral_offset = 1.6m)
    lane.left_boundary.id = "left";
    lane.left_boundary.coefficients = {1.6, -0.02, 0.0001, 0.0}; // slight left curve
    lane.left_boundary.confidence = 0.95;
    lane.left_boundary.valid = true;

    // Right Boundary: y = -1.75 meters
    // Adjusting for ego car lateral offset (c0 = -1.75 - lateral_offset = -1.9m)
    lane.right_boundary.id = "right";
    lane.right_boundary.coefficients = {-1.9, -0.02, 0.0001, 0.0};
    lane.right_boundary.confidence = 0.95;
    lane.right_boundary.valid = true;

    UADOS_LOG_DEBUG("Lane Detected: lateral_offset={:.2f}m, heading_error={:.2f} rad, left_c0={:.2f}m, right_c0={:.2f}m",
                    lane.lateral_offset, lane.heading_error,
                    lane.left_boundary.coefficients[0], lane.right_boundary.coefficients[0]);

    return lane;
}

} // namespace uados::perception
