#include "uados/sensors/camera_driver.hpp"
#include "uados/sensors/lidar_driver.hpp"
#include "uados/sensors/radar_driver.hpp"
#include "uados/sensors/gps_driver.hpp"
#include "uados/sensors/imu_driver.hpp"
#include <gtest/gtest.h>

using namespace uados;
using namespace uados::sensors;

TEST(SensorDriversTest, CameraDriverAcquisition) {
    CameraDriver camera;
    uados::core::Config config;
    
    ASSERT_EQ(camera.init(config), Status::Ok);
    ASSERT_EQ(camera.start(), Status::Ok);
    ASSERT_TRUE(camera.is_active());

    auto data = camera.read();
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->sensor_type, SensorType::Camera);
    EXPECT_EQ(data->sensor_id, "camera_front");

    auto frame = std::dynamic_pointer_cast<ImageFrame>(data);
    ASSERT_NE(frame, nullptr);
    EXPECT_EQ(frame->width, 640);
    EXPECT_EQ(frame->height, 480);
    EXPECT_EQ(frame->data.size(), 640 * 480 * 3);

    ASSERT_EQ(camera.stop(), Status::Ok);
}

TEST(SensorDriversTest, LiDARDriverAcquisition) {
    LiDARDriver lidar;
    uados::core::Config config;

    ASSERT_EQ(lidar.init(config), Status::Ok);
    ASSERT_EQ(lidar.start(), Status::Ok);

    auto data = lidar.read();
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->sensor_type, SensorType::LiDAR);

    auto cloud = std::dynamic_pointer_cast<PointCloud>(data);
    ASSERT_NE(cloud, nullptr);
    EXPECT_EQ(cloud->width, 180);
    EXPECT_EQ(cloud->height, 16);
    EXPECT_EQ(cloud->points.size(), 16 * 180);

    // Verify a random point's bounds
    const auto& pt = cloud->points[0];
    EXPECT_GT(pt.intensity, 0.0f);
    EXPECT_EQ(pt.ring, 0);

    ASSERT_EQ(lidar.stop(), Status::Ok);
}

TEST(SensorDriversTest, RadarDriverAcquisition) {
    RadarDriver radar;
    uados::core::Config config;

    ASSERT_EQ(radar.init(config), Status::Ok);
    ASSERT_EQ(radar.start(), Status::Ok);

    auto data = radar.read();
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->sensor_type, SensorType::Radar);

    auto scan = std::dynamic_pointer_cast<RadarScan>(data);
    ASSERT_NE(scan, nullptr);
    ASSERT_EQ(scan->detections.size(), 2);
    EXPECT_DOUBLE_EQ(scan->detections[0].range, 15.0);
    EXPECT_DOUBLE_EQ(scan->detections[1].range, 8.0);

    ASSERT_EQ(radar.stop(), Status::Ok);
}

TEST(SensorDriversTest, GPSDriverAcquisition) {
    GPSDriver gps;
    uados::core::Config config;

    ASSERT_EQ(gps.init(config), Status::Ok);
    ASSERT_EQ(gps.start(), Status::Ok);

    auto data = gps.read();
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->sensor_type, SensorType::GPS);

    auto fix = std::dynamic_pointer_cast<GPSFix>(data);
    ASSERT_NE(fix, nullptr);
    EXPECT_NEAR(fix->coordinate.latitude, 37.7749, 0.01);
    EXPECT_NEAR(fix->coordinate.longitude, -122.4194, 0.01);
    EXPECT_EQ(fix->fix_type, GPSFix::FixType::RTK_Fixed);

    ASSERT_EQ(gps.stop(), Status::Ok);
}

TEST(SensorDriversTest, IMUDriverAcquisition) {
    IMUDriver imu;
    uados::core::Config config;

    ASSERT_EQ(imu.init(config), Status::Ok);
    ASSERT_EQ(imu.start(), Status::Ok);

    auto data = imu.read();
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->sensor_type, SensorType::IMU);

    auto reading = std::dynamic_pointer_cast<IMUReading>(data);
    ASSERT_NE(reading, nullptr);
    EXPECT_NEAR(reading->linear_acceleration.az, 9.81, 0.5); // gravity check

    ASSERT_EQ(imu.stop(), Status::Ok);
}
