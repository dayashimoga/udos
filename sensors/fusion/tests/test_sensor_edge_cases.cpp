#include "uados/sensors/sensor_fusion.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

using namespace uados;
using namespace uados::sensors;

/// @file test_sensor_edge_cases.cpp
/// @brief Boundary and edge-case tests for sensor drivers and EKF fusion.

// ============================================================================
// 1. GPS Edge Cases
// ============================================================================

TEST(SensorEdgeCases, GPSZeroSatelliteFix) {
    SensorFusion fusion;
    uados::core::Config config;
    
    fusion.init(config);
    fusion.start();

    // GPS fix with zero satellites (should still bootstrap, but with low confidence)
    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 37.7749;
    gps.coordinate.longitude = -122.4194;
    gps.speed = 0.0;
    gps.heading = 0.0;
    gps.horizontal_accuracy = 100.0; // very poor accuracy
    gps.satellites = 0;
    gps.fix_type = GPSFix::FixType::NoFix;

    // Should not crash even with degenerate GPS data
    fusion.correct(gps);
    auto estimate = fusion.state_estimate();
    
    EXPECT_TRUE(std::isfinite(estimate.pose.position.x));
    EXPECT_TRUE(std::isfinite(estimate.pose.position.y));
    EXPECT_TRUE(std::isfinite(estimate.velocity.vx));

    fusion.stop();
}

TEST(SensorEdgeCases, GPSPolarCoordinates) {
    // Test with GPS coordinates at the poles (latitude ≈ ±90)
    // The cosine(lat) projection factor approaches zero at poles
    SensorFusion fusion;
    uados::core::Config config;
    
    fusion.init(config);
    fusion.start();

    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 89.9999; // near north pole
    gps.coordinate.longitude = 0.0;
    gps.speed = 0.0;
    gps.heading = 0.0;
    gps.horizontal_accuracy = 1.0;
    gps.fix_type = GPSFix::FixType::Fix3D;

    fusion.correct(gps);
    auto estimate = fusion.state_estimate();

    // Must not produce NaN/Inf despite cos(90°) ≈ 0
    EXPECT_TRUE(std::isfinite(estimate.pose.position.x));
    EXPECT_TRUE(std::isfinite(estimate.pose.position.y));

    fusion.stop();
}

// ============================================================================
// 2. IMU Edge Cases
// ============================================================================

TEST(SensorEdgeCases, IMUZeroTimestampDelta) {
    SensorFusion fusion;
    uados::core::Config config;
    
    fusion.init(config);
    fusion.start();

    // Bootstrap
    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 37.7749;
    gps.coordinate.longitude = -122.4194;
    gps.speed = 5.0;
    gps.heading = 0.0;
    gps.horizontal_accuracy = 0.5;
    fusion.correct(gps);

    // Send two IMU readings with identical timestamps (dt = 0)
    IMUReading imu1;
    imu1.timestamp = gps.timestamp;
    imu1.linear_acceleration.ax = 2.0;
    imu1.linear_acceleration.ay = 0.0;
    imu1.linear_acceleration.az = 9.81;

    IMUReading imu2;
    imu2.timestamp = gps.timestamp; // SAME timestamp
    imu2.linear_acceleration.ax = 3.0;
    imu2.linear_acceleration.ay = 0.0;
    imu2.linear_acceleration.az = 9.81;

    fusion.predict(imu1);
    fusion.predict(imu2); // dt=0, should be a no-op or handled gracefully

    auto estimate = fusion.state_estimate();
    EXPECT_TRUE(std::isfinite(estimate.velocity.vx));
    EXPECT_TRUE(std::isfinite(estimate.pose.position.x));

    fusion.stop();
}

TEST(SensorEdgeCases, IMUExtremeAcceleration) {
    SensorFusion fusion;
    uados::core::Config config;
    
    fusion.init(config);
    fusion.start();

    // Bootstrap
    GPSFix gps;
    gps.timestamp = Clock::now();
    gps.coordinate.latitude = 37.7749;
    gps.coordinate.longitude = -122.4194;
    gps.speed = 0.0;
    gps.heading = 0.0;
    gps.horizontal_accuracy = 0.5;
    fusion.correct(gps);

    // Extreme acceleration spike (sensor failure scenario)
    IMUReading imu;
    imu.timestamp = gps.timestamp + std::chrono::milliseconds(100);
    imu.linear_acceleration.ax = 500.0; // impossibly high for a vehicle
    imu.linear_acceleration.ay = 300.0;
    imu.linear_acceleration.az = 9.81;

    fusion.predict(imu);

    auto estimate = fusion.state_estimate();
    // Must remain finite even under extreme inputs
    EXPECT_TRUE(std::isfinite(estimate.velocity.vx));
    EXPECT_TRUE(std::isfinite(estimate.velocity.vy));
    EXPECT_TRUE(std::isfinite(estimate.pose.position.x));

    fusion.stop();
}

// ============================================================================
// 3. Fusion Lifecycle Edge Cases
// ============================================================================

TEST(SensorEdgeCases, FusionDoubleInitIdempotent) {
    SensorFusion fusion;
    uados::core::Config config;
    
    EXPECT_EQ(fusion.init(config), Status::Ok);
    EXPECT_EQ(fusion.init(config), Status::Ok); // re-init should be safe

    EXPECT_EQ(fusion.start(), Status::Ok);
    EXPECT_EQ(fusion.start(), Status::Ok); // double start

    EXPECT_EQ(fusion.stop(), Status::Ok);
    EXPECT_EQ(fusion.stop(), Status::Ok); // double stop
}

TEST(SensorEdgeCases, FusionPredictBeforeBootstrap) {
    SensorFusion fusion;
    uados::core::Config config;
    
    fusion.init(config);
    fusion.start();

    // Predict before any GPS correction (bootstrap hasn't occurred)
    IMUReading imu;
    imu.timestamp = Clock::now();
    imu.linear_acceleration.ax = 1.0;
    imu.linear_acceleration.ay = 0.0;
    imu.linear_acceleration.az = 9.81;

    // Should not crash
    fusion.predict(imu);

    auto estimate = fusion.state_estimate();
    EXPECT_TRUE(std::isfinite(estimate.pose.position.x));

    fusion.stop();
}
