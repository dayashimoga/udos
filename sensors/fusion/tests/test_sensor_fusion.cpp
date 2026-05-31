#include "uados/sensors/sensor_fusion.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

using namespace uados;
using namespace uados::sensors;

TEST(SensorFusionTest, EKFInitialization) {
    SensorFusion fusion;
    uados::core::Config config;
    
    ASSERT_EQ(fusion.init(config), Status::Ok);
    ASSERT_EQ(fusion.start(), Status::Ok);

    // Initial state estimate should be zero (since bootstrap hasn't occurred yet)
    auto estimate = fusion.state_estimate();
    EXPECT_DOUBLE_EQ(estimate.pose.position.x, 0.0);
    EXPECT_DOUBLE_EQ(estimate.pose.position.y, 0.0);

    // Bootstrap EKF with first GPS reading (at origin 37.7749, -122.4194)
    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 37.7749;
    gps.coordinate.longitude = -122.4194;
    gps.speed = 5.0;
    gps.heading = 0.0; // Heading East (yaw = 0)
    gps.horizontal_accuracy = 0.1;
    gps.fix_type = GPSFix::FixType::RTK_Fixed;

    fusion.correct(gps);

    estimate = fusion.state_estimate();
    EXPECT_NEAR(estimate.pose.position.x, 0.0, 0.001);
    EXPECT_NEAR(estimate.pose.position.y, 0.0, 0.001);
    EXPECT_NEAR(estimate.velocity.vx, 5.0, 0.001);
    EXPECT_NEAR(estimate.velocity.vy, 0.0, 0.001);

    fusion.stop();
}

TEST(SensorFusionTest, EKFPredictionStep) {
    SensorFusion fusion;
    uados::core::Config config;

    fusion.init(config);
    fusion.start();

    // 1. Initialize
    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 37.7749;
    gps.coordinate.longitude = -122.4194;
    gps.speed = 0.0; // Stationary
    gps.heading = 0.0;
    gps.horizontal_accuracy = 0.1;
    fusion.correct(gps);

    // 2. Perform EKF prediction with positive acceleration ax = 2.0 m/s^2
    IMUReading imu;
    imu.timestamp = gps.timestamp + std::chrono::milliseconds(100); // dt = 0.1s
    imu.linear_acceleration.ax = 2.0;
    imu.linear_acceleration.ay = 0.0;
    imu.linear_acceleration.az = 9.81;

    fusion.predict(imu);

    auto estimate = fusion.state_estimate();
    // Velocity vx = ax * dt = 2.0 * 0.1 = 0.2 m/s
    EXPECT_NEAR(estimate.velocity.vx, 0.2, 0.01);
    EXPECT_DOUBLE_EQ(estimate.velocity.vy, 0.0);
    // Position px = 0.5 * ax * dt^2 = 0.5 * 2 * 0.01 = 0.01 m
    EXPECT_NEAR(estimate.pose.position.x, 0.01, 0.001);

    fusion.stop();
}

TEST(SensorFusionTest, EKFCorrectionConvergence) {
    SensorFusion fusion;
    uados::core::Config config;

    fusion.init(config);
    fusion.start();

    // 1. Initialize
    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 37.7749;
    gps.coordinate.longitude = -122.4194;
    gps.speed = 10.0; // moving East at 10m/s
    gps.heading = 0.0;
    gps.horizontal_accuracy = 0.1;
    fusion.correct(gps);

    // 2. Apply IMU prediction step
    IMUReading imu;
    imu.timestamp = gps.timestamp + std::chrono::milliseconds(100); // dt = 0.1s
    imu.linear_acceleration.ax = 0.0; // constant speed
    imu.linear_acceleration.ay = 0.0;
    imu.linear_acceleration.az = 9.81;
    fusion.predict(imu);

    // Position should be x = 10m/s * 0.1s = 1.0m
    auto estimate = fusion.state_estimate();
    EXPECT_NEAR(estimate.pose.position.x, 1.0, 0.05);

    // 3. Receive a noisy GPS correction update (GPS reports 1.1m instead of 1.0m)
    GPSFix gps_corr;
    gps_corr.timestamp = imu.timestamp;
    // lon offset representing 1.1m East projection
    gps_corr.coordinate.latitude = 37.7749;
    // 1.1m / (111320.0 * cos(37.7749)) = 1.1 / 87990.0 = 1.25e-5 deg
    gps_corr.coordinate.longitude = -122.4194 + 1.25e-5 / std::cos(37.7749 * 3.14159 / 180.0);
    gps_corr.speed = 10.0;
    gps_corr.heading = 0.0;
    gps_corr.horizontal_accuracy = 0.15; // 15cm precision

    fusion.correct(gps_corr);

    // Fused state should compromise between prediction (1.0m) and correction (1.1m)
    estimate = fusion.state_estimate();
    EXPECT_GT(estimate.pose.position.x, 1.0);
    EXPECT_LT(estimate.pose.position.x, 1.11);

    fusion.stop();
}
