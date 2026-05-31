#include "uados/localization/pose_estimator.hpp"
#include "uados/localization/hdmap_engine.hpp"
#include "uados/localization/slam_engine.hpp"
#include <gtest/gtest.h>

using namespace uados;
using namespace uados::localization;

TEST(LocalizationTest, PoseEstimatorCoordinateProjection) {
    PoseEstimator estimator;
    uados::core::Config config;

    ASSERT_EQ(estimator.init(config), Status::Ok);
    ASSERT_EQ(estimator.start(), Status::Ok);

    // Initial origin: 37.7749, -122.4194 (San Francisco)
    double x = 0.0, y = 0.0;
    
    // 1. Project origin to local, should be (0, 0)
    estimator.geodesic_to_local(37.7749, -122.4194, 10.0, x, y);
    EXPECT_NEAR(x, 0.0, 0.01);
    EXPECT_NEAR(y, 0.0, 0.01);

    // 2. Project coordinate 100m East, 100m North
    // dlat = 100m / R = 1.5678e-5 rad = 8.98e-4 deg
    // dlon = 100m / (R * cos(lat))
    double test_lat = 37.7749 + (100.0 / 6378137.0) * (180.0 / 3.14159265);
    double test_lon = -122.4194 + (100.0 / (6378137.0 * std::cos(37.7749 * 3.14159265 / 180.0))) * (180.0 / 3.14159265);

    estimator.geodesic_to_local(test_lat, test_lon, 10.0, x, y);
    EXPECT_NEAR(x, 100.0, 0.5);
    EXPECT_NEAR(y, 100.0, 0.5);

    // 3. Project back to Geodesic
    double lat = 0.0, lon = 0.0;
    estimator.local_to_geodesic(100.0, 100.0, lat, lon);
    EXPECT_NEAR(lat, test_lat, 0.0001);
    EXPECT_NEAR(lon, test_lon, 0.0001);

    estimator.stop();
}

TEST(LocalizationTest, HDMapEngineQueries) {
    HDMapEngine hdmap;
    uados::core::Config config;

    ASSERT_EQ(hdmap.init(config), Status::Ok);
    ASSERT_EQ(hdmap.start(), Status::Ok);

    // 1. Query at x=10m (straight road lanelet_1001)
    Pose p1;
    p1.position = {10.0, 0.2, 0.0}; // 20cm left of center
    
    auto info1 = hdmap.get_nearest_lanelet(p1);
    EXPECT_EQ(info1.id, "lanelet_1001");
    EXPECT_DOUBLE_EQ(info1.speed_limit_mps, 13.8); // 50 km/h
    EXPECT_NEAR(info1.distance_to_centerline, 0.2, 0.001);
    EXPECT_FALSE(info1.is_intersection);
    EXPECT_FALSE(info1.has_stop_line);

    // 2. Query at x=50m (lanelet_1002, approaching stop line)
    Pose p2;
    p2.position = {50.0, 0.0, 0.0};
    
    auto info2 = hdmap.get_nearest_lanelet(p2);
    EXPECT_EQ(info2.id, "lanelet_1002");
    EXPECT_DOUBLE_EQ(info2.speed_limit_mps, 8.33); // 30 km/h
    EXPECT_TRUE(info2.has_stop_line);
    // stop line is at 58.0m -> distance = 58.0 - 50.0 = 8.0m
    EXPECT_NEAR(info2.stop_line_distance, 8.0, 0.001);

    hdmap.stop();
}

TEST(LocalizationTest, SLAMEngineDeadReckoning) {
    SLAMEngine slam;
    uados::core::Config config;

    ASSERT_EQ(slam.init(config), Status::Ok);
    ASSERT_EQ(slam.start(), Status::Ok);

    // Reset reference to (10.0, 20.0) yaw = 0.0
    Pose start;
    start.position = {10.0, 20.0, 0.0};
    start.orientation = Quat::Identity();
    slam.reset_pose(start);

    // 1. Integrate straight motion: speed = 10m/s, yaw_rate = 0, dt = 0.1s
    // Should move 1.0m East -> x = 11.0, y = 20.0
    auto p1 = slam.update_odometry(10.0, 0.0, 0.1);
    EXPECT_NEAR(p1.position.x, 11.0, 0.001);
    EXPECT_NEAR(p1.position.y, 20.0, 0.001);

    // 2. Integrate turning motion: speed = 10m/s, yaw_rate = PI/2 rad/s (90 deg/s), dt = 1.0s
    // Yaw goes from 0.0 to PI/2 (pointing North). Position integrates diagonally.
    slam.reset_pose(start); // reset to (10, 20), yaw = 0
    slam.update_odometry(10.0, 1.570796, 1.0);
    
    auto p2 = slam.current_pose();
    double yaw = p2.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    EXPECT_NEAR(yaw, 1.570796, 0.01); // 90 deg pointing North
    EXPECT_GT(p2.position.x, 10.0);    // moved East
    EXPECT_GT(p2.position.y, 20.0);    // moved North

    slam.stop();
}

