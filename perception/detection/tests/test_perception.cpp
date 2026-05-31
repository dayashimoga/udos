#include "uados/perception/object_detector.hpp"
#include "uados/perception/object_tracker.hpp"
#include "uados/perception/lane_detector.hpp"
#include "uados/perception/traffic_light_detector.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

using namespace uados;
using namespace uados::perception;

TEST(PerceptionTest, ObjectDetectorProjection) {
    ObjectDetector detector;
    uados::core::Config config;
    
    ASSERT_EQ(detector.init(config), Status::Ok);
    ASSERT_EQ(detector.start(), Status::Ok);

    sensors::ImageFrame frame;
    frame.width = 640;
    frame.height = 480;
    frame.timestamp = Clock::now();

    auto objects = detector.detect(frame);
    // InferenceEngine returns 2 mocked objects (Car, Pedestrian)
    ASSERT_EQ(objects.size(), 2);

    // Verify Car projection details
    const auto& car = objects[0];
    EXPECT_EQ(car.object_class, ObjectClass::Car);
    EXPECT_NEAR(car.confidence, 0.88, 0.01);
    
    // Depth estimation check:
    // focal_length = 500, car_prior_width = 1.7, raw box_width = 150
    // depth = (500 * 1.7) / 150 = 5.67m
    EXPECT_NEAR(car.position.x, 5.67, 0.1);
    // Lateral: cx = 320, box_cx = 320, px = 0.0m
    EXPECT_NEAR(car.position.y, 0.0, 0.1);

    // Verify Pedestrian projection details
    const auto& ped = objects[1];
    EXPECT_EQ(ped.object_class, ObjectClass::Pedestrian);
    EXPECT_NEAR(ped.confidence, 0.75, 0.01);
    
    // Depth: focal = 500, ped_prior_width = 0.5, raw box_width = 40
    // depth = (500 * 0.5) / 40 = 6.25m
    EXPECT_NEAR(ped.position.x, 6.25, 0.1);

    detector.stop();
}

TEST(PerceptionTest, ObjectTrackerMOT) {
    ObjectTracker tracker;
    uados::core::Config config;

    ASSERT_EQ(tracker.init(config), Status::Ok);
    ASSERT_EQ(tracker.start(), Status::Ok);

    // 1. Birth: detection at (5.0, 0.0, 0.0)
    std::vector<DetectedObject> frame1;
    DetectedObject o1;
    o1.id = 1;
    o1.object_class = ObjectClass::Car;
    o1.position = {5.0, 0.0, 0.0};
    o1.confidence = 0.9;
    frame1.push_back(o1);

    auto tracks1 = tracker.track(frame1);
    ASSERT_EQ(tracks1.size(), 1);
    EXPECT_EQ(tracks1[0].id, 100); // First persistent ID
    EXPECT_DOUBLE_EQ(tracks1[0].velocity.vx, 0.0);

    // 2. Association: detection moves to (5.5, 0.0, 0.0) after 0.1s
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::vector<DetectedObject> frame2;
    DetectedObject o2 = o1;
    o2.position = {5.5, 0.0, 0.0};
    frame2.push_back(o2);

    auto tracks2 = tracker.track(frame2);
    ASSERT_EQ(tracks2.size(), 1);
    EXPECT_EQ(tracks2[0].id, 100); // Retained ID
    // Velocity: (5.5 - 5.0) / 0.1s = 5.0m/s. Smooth factor Ema makes it ~1.5m/s on step 1
    EXPECT_GT(tracks2[0].velocity.vx, 0.5);

    // 3. Death: frame with no detections, track misses frames and dies
    std::vector<DetectedObject> empty_frame;
    for (int i = 0; i < 6; ++i) {
        tracker.track(empty_frame); // Increments miss count
    }

    // A new frame at same spot will get a NEW birth ID
    auto tracks3 = tracker.track(frame1);
    ASSERT_EQ(tracks3.size(), 1);
    EXPECT_EQ(tracks3[0].id, 101); // New track ID since track 100 died

    tracker.stop();
}

TEST(PerceptionTest, LaneDetectorFittings) {
    LaneDetector detector;
    uados::core::Config config;

    ASSERT_EQ(detector.init(config), Status::Ok);
    ASSERT_EQ(detector.start(), Status::Ok);

    sensors::ImageFrame frame;
    auto lane = detector.detect(frame);

    EXPECT_DOUBLE_EQ(lane.lane_width, 3.5);
    EXPECT_DOUBLE_EQ(lane.lateral_offset, 0.15);
    EXPECT_TRUE(lane.left_boundary.valid);
    EXPECT_TRUE(lane.right_boundary.valid);

    // Left c0 = 1.75 - 0.15 = 1.6
    EXPECT_DOUBLE_EQ(lane.left_boundary.coefficients[0], 1.6);
    // Right c0 = -1.75 - 0.15 = -1.9
    EXPECT_DOUBLE_EQ(lane.right_boundary.coefficients[0], -1.9);

    detector.stop();
}

TEST(PerceptionTest, TrafficLightColorCycles) {
    TrafficLightDetector detector;
    uados::core::Config config;

    ASSERT_EQ(detector.init(config), Status::Ok);
    ASSERT_EQ(detector.start(), Status::Ok);

    sensors::ImageFrame frame;
    
    // Test cycling frames to simulate state shifts
    auto lights = detector.detect(frame);
    ASSERT_EQ(lights.size(), 1);
    EXPECT_EQ(lights[0].state, TrafficLightState::Green); // Initial phase is Green

    // Fast-forward frame sequence to Yellow phase (phase > 100)
    for (int i = 0; i < 100; ++i) {
        detector.detect(frame);
    }
    
    lights = detector.detect(frame);
    ASSERT_EQ(lights.size(), 1);
    EXPECT_EQ(lights[0].state, TrafficLightState::Yellow);

    detector.stop();
}
