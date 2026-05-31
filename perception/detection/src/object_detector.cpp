#include "uados/perception/object_detector.hpp"
#include "uados/logging.hpp"

namespace uados::perception {

UADOS_DECLARE_LOGGER("perception.detection")

Status ObjectDetector::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing 3D Object Detector...");

    if (config) {
        if (config["focal_length_x"]) {
            fx_ = config["focal_length_x"].as<double>();
        }
        if (config["focal_length_y"]) {
            fy_ = config["focal_length_y"].as<double>();
        }
        if (config["center_x"]) {
            cx_ = config["center_x"].as<double>();
        }
        if (config["center_y"]) {
            cy_ = config["center_y"].as<double>();
        }
        if (config["model_path"]) {
            auto model_path = config["model_path"].as<std::string>();
            engine_.load_model(model_path);
        } else {
            engine_.load_model("models/yolov8s_3d.onnx");
        }
    } else {
        engine_.load_model("models/yolov8s_3d.onnx");
    }

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("Object Detector initialized: intrinsics fx={:.1f}, cx={:.1f}", fx_, cx_);
    return Status::Ok;
}

Status ObjectDetector::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status ObjectDetector::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

std::vector<DetectedObject> ObjectDetector::detect(const sensors::ImageFrame& frame) {
    std::lock_guard lock(mutex_);
    if (!active_) return {};

    // 1. Run forward model pass
    std::vector<float> input(frame.width * frame.height * 3, 0.5f); // Normalized dummy input
    auto raw_detections = engine_.run_inference(input, {1, 3, frame.width, frame.height});

    std::vector<DetectedObject> objects;
    
    // Each raw detection takes 6 floats: [cx, cy, w, h, conf, class_id]
    size_t num_detections = raw_detections.size() / 6;
    objects.reserve(num_detections);

    for (size_t i = 0; i < num_detections; ++i) {
        size_t idx = i * 6;
        double bbox_cx = raw_detections[idx];
        double bbox_cy = raw_detections[idx + 1];
        double bbox_w  = raw_detections[idx + 2];
        double bbox_h  = raw_detections[idx + 3];
        double conf    = raw_detections[idx + 4];
        double class_id = raw_detections[idx + 5];

        ObjectClass cls = ObjectClass::Unknown;
        if (class_id == 1.0) cls = ObjectClass::Car;
        else if (class_id == 6.0) cls = ObjectClass::Pedestrian;

        // 2. Depth estimation by prior geometric size
        double depth = estimate_depth(cls, bbox_w);

        // 3. Pinhole Projection: project 2D image coordinates to 3D camera coordinates
        // Z is forward distance
        // X is lateral distance (positive right)
        // Y is vertical height (positive down, offset from optical center)
        double pz = depth;
        double px = ((bbox_cx - cx_) * pz) / fx_;
        double py = ((bbox_cy - cy_) * pz) / fy_;

        DetectedObject obj;
        obj.id = next_object_id_++;
        obj.object_class = cls;
        obj.confidence = conf;
        
        // Map camera frame to standard vehicle frame:
        // Vehicle X: Forward (pz)
        // Vehicle Y: Left (-px)
        // Vehicle Z: Up (-py)
        obj.position.x = pz;
        obj.position.y = -px;
        obj.position.z = -py;

        if (cls == ObjectClass::Car) {
            obj.dimensions = {4.5, 1.7, 1.4}; // typical sedan dimensions
        } else {
            obj.dimensions = {0.6, 0.5, 1.75}; // typical pedestrian dimensions
        }

        obj.orientation = Quat::Identity();
        obj.velocity = {0.0, 0.0, 0.0};
        obj.timestamp = frame.timestamp;

        objects.push_back(obj);

        UADOS_LOG_DEBUG("Detected {}: id={}, position=({:.2f}, {:.2f}, {:.2f}), conf={:.2f}",
                        cls == ObjectClass::Car ? "Car" : "Pedestrian",
                        obj.id, obj.position.x, obj.position.y, obj.position.z, obj.confidence);
    }

    return objects;
}

double ObjectDetector::estimate_depth(ObjectClass cls, double box_width) const noexcept {
    if (box_width <= 0.0) return 30.0;

    double prior_width = 1.7; // default car width
    if (cls == ObjectClass::Pedestrian) {
        prior_width = 0.5; // pedestrian width
    }

    // Depth-by-Prior Size standard formula
    return (fx_ * prior_width) / box_width;
}

} // namespace uados::perception
