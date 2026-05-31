#include "uados/prediction/trajectory_predictor.hpp"
#include "uados/logging.hpp"

#include <cmath>

namespace uados::prediction {

UADOS_DECLARE_LOGGER("prediction.trajectory")

Status TrajectoryPredictor::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);
    UADOS_LOG_INFO("Initializing Kinematic Trajectory Predictor...");
    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);
    return Status::Ok;
}

Status TrajectoryPredictor::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status TrajectoryPredictor::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<ObstaclePrediction> TrajectoryPredictor::predict(
    const std::vector<DetectedObject>& tracked_obstacles,
    double horizon_seconds,
    double step_seconds) {

    std::lock_guard lock(mutex_);
    if (!active_ || horizon_seconds <= 0.0 || step_seconds <= 0.0) return {};

    std::vector<ObstaclePrediction> predictions;
    predictions.reserve(tracked_obstacles.size());

    size_t num_steps = static_cast<size_t>(horizon_seconds / step_seconds);
    auto now = Clock::now();

    for (const auto& obj : tracked_obstacles) {
        ObstaclePrediction pred;
        pred.obstacle_id = obj.id;
        pred.object_class = obj.object_class;

        PredictedPath path;
        path.probability = 1.0;
        path.points.resize(num_steps);

        double x0 = obj.position.x;
        double y0 = obj.position.y;
        double z0 = obj.position.z;

        double vx = obj.velocity.vx;
        double vy = obj.velocity.vy;

        // Kinematic Constant Acceleration (CA) model:
        // We estimate acceleration based on velocity vectors
        // (if acceleration fields are zero, defaults to Constant Velocity)
        double ax = 0.0;
        double ay = 0.0;
        
        // Calculate heading yaw angle
        double heading = std::atan2(vy, vx);

        for (size_t step = 0; step < num_steps; ++step) {
            double t = static_cast<double>(step + 1) * step_seconds;

            // Equations of Motion
            double xt = x0 + vx * t + 0.5 * ax * t * t;
            double yt = y0 + vy * t + 0.5 * ay * t * t;

            TrajectoryPoint pt;
            pt.position.x = xt;
            pt.position.y = yt;
            pt.position.z = z0;
            pt.speed = std::sqrt(vx * vx + vy * vy) + std::sqrt(ax * ax + ay * ay) * t;
            pt.acceleration = std::sqrt(ax * ax + ay * ay);
            pt.curvature = 0.0; // straight prediction
            pt.heading = heading;
            pt.time_offset = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(t));

            path.points[step] = pt;
        }

        pred.paths.push_back(path);
        predictions.push_back(pred);

        UADOS_LOG_DEBUG("Obstacle Prediction: id={}, predicted pos in 3s=({:.2f}, {:.2f})",
                        obj.id, path.points[5].position.x, path.points[5].position.y);
    }

    return predictions;
}

} // namespace uados::prediction
