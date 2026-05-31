#include "uados/digital_twin/vehicle_twin.hpp"
#include "uados/digital_twin/sensor_twin.hpp"

#include <gtest/gtest.h>
#include <chrono>

using namespace uados;
using namespace uados::digital_twin;

class DigitalTwinSubsystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        uados::core::Config mock_config;
        
        ASSERT_EQ(vehicle_twin_.init(mock_config), Status::Ok);
        ASSERT_EQ(sensor_twin_.init(mock_config), Status::Ok);

        ASSERT_EQ(vehicle_twin_.start(), Status::Ok);
        ASSERT_EQ(sensor_twin_.start(), Status::Ok);
    }

    void TearDown() override {
        EXPECT_EQ(vehicle_twin_.stop(), Status::Ok);
        EXPECT_EQ(sensor_twin_.stop(), Status::Ok);
    }

    VehicleDigitalTwin vehicle_twin_;
    SensorTwin sensor_twin_;
};

// ============================================================================
// 1. Vehicle Digital Twin dynamics Tests
// ============================================================================

TEST_F(DigitalTwinSubsystemTest, TestVehicleDynamicsAcceleration) {
    // Start vehicle at rest
    Pose initial_pose;
    initial_pose.position = {0.0, 0.0, 0.0};
    initial_pose.orientation = Quat::Identity();
    vehicle_twin_.reset(initial_pose, 0.0);

    // Apply acceleration for 1.0 second (dt = 0.1s, 10 steps)
    for (int i = 0; i < 10; ++i) {
        vehicle_twin_.step(0.0, 2.0, 0.1);
    }

    auto state = vehicle_twin_.get_state();

    // Speed should integrate: v = u + a*t = 0 + 2.0*1.0 = 2.0 m/s
    EXPECT_NEAR(state.velocity.vx, 2.0, 0.05);
    // Position x should advance: s = 0.5 * a * t^2 = 1.0m
    EXPECT_NEAR(state.position.x, 1.0, 0.05);
    EXPECT_NEAR(state.position.y, 0.0, 0.01);
}

TEST_F(DigitalTwinSubsystemTest, TestVehicleDynamicsAckermannTurning) {
    // Start vehicle moving at 5.0 m/s
    Pose initial_pose;
    initial_pose.position = {0.0, 0.0, 0.0};
    initial_pose.orientation = Quat::Identity();
    vehicle_twin_.reset(initial_pose, 5.0);

    // Apply 0.3 rad steering angle (approx 17 degrees) and step physics
    // Step turning dynamics for 1.0s
    for (int i = 0; i < 10; ++i) {
        vehicle_twin_.step(0.3, 0.0, 0.1);
    }

    auto state = vehicle_twin_.get_state();

    // Heading yaw must change due to turning dynamics (Ackermann slip)
    double yaw = state.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    if (std::isnan(yaw)) {
        yaw = 0.0;
    }
    
    // Vehicle should be turning left, so yaw heading must be positive
    EXPECT_GT(yaw, 0.1);
    // Vehicle y coordinate should drift left (positive ENU y axle)
    EXPECT_GT(state.position.y, 0.05);
    EXPECT_GT(state.position.x, 2.0); // still advanced longitudinal distance
}

// ============================================================================
// 2. Sensor Digital Twin Projection and Noise Tests
// ============================================================================

TEST_F(DigitalTwinSubsystemTest, TestCameraPinholeProjectionVisibility) {
    // Camera is mounted forward (x=2.0m, y=0.0m, z=1.5m)
    Pose cam_pose;
    cam_pose.position = {2.0, 0.0, 1.5};
    cam_pose.orientation = Quat::Identity();

    // Point 10 meters directly in front of camera, slightly right and lower
    Position3D pt;
    pt.x = 12.0;  // 10m relative to cam_x
    pt.y = -1.0;  // 1m relative to cam_y (right is negative y in camera frame)
    pt.z = 0.5;   // 1m lower than cam_z

    auto pixel = sensor_twin_.project_to_camera(pt, cam_pose);

    // Point must project successfully into image bounds
    EXPECT_TRUE(pixel.visible);
    EXPECT_GT(pixel.u, 0.0);
    EXPECT_GT(pixel.v, 0.0);

    // Test coordinates behind camera view
    Position3D behind_pt;
    behind_pt.x = 0.0; // behind camera
    behind_pt.y = 0.0;
    behind_pt.z = 1.5;

    auto invalid_pixel = sensor_twin_.project_to_camera(behind_pt, cam_pose);
    EXPECT_FALSE(invalid_pixel.visible);
}

TEST_F(DigitalTwinSubsystemTest, TestRadarSimulationScanning) {
    // Ego vehicle is at rest at origin
    VehicleState ego_state;
    ego_state.position = {0.0, 0.0, 0.0};
    ego_state.orientation = Quat::Identity();
    ego_state.velocity = {0.0, 0.0, 0.0};

    // Obstacle 1: 15.0m directly ahead (inside radar FOV)
    DetectedObject target1;
    target1.id = 101;
    target1.object_class = ObjectClass::Car;
    target1.confidence = 0.9;
    target1.position = {15.0, 0.0, 0.0};
    target1.velocity = {5.0, 0.0, 0.0};
    target1.dimensions = {4.5, 1.8, 1.5};

    // Obstacle 2: 120.0m ahead (outside radar maximum range)
    DetectedObject target2;
    target2.id = 102;
    target2.object_class = ObjectClass::Car;
    target2.confidence = 0.9;
    target2.position = {120.0, 0.0, 0.0};
    target2.velocity = {0.0, 0.0, 0.0};

    // Obstacle 3: 15.0m relative behind (outside radar FOV angle boundaries)
    DetectedObject target3;
    target3.id = 103;
    target3.object_class = ObjectClass::Pedestrian;
    target3.confidence = 0.85;
    target3.position = {-15.0, 0.0, 0.0};
    target3.velocity = {0.0, 0.0, 0.0};

    std::vector<DetectedObject> traffic = {target1, target2, target3};

    auto radar_detections = sensor_twin_.simulate_radar(ego_state, traffic);

    // Radar should drop target2 (out of range) and target3 (out of FOV)
    ASSERT_EQ(radar_detections.size(), 1);
    EXPECT_EQ(radar_detections[0].id, 101);

    // Noisy position should resemble original position within noise bounds (+/- 3 sigma)
    EXPECT_NEAR(radar_detections[0].position.x, 15.0, 0.45); // std_dev = 0.15
    EXPECT_NEAR(radar_detections[0].velocity.vx, 5.0, 0.75);  // std_dev = 0.25
}
