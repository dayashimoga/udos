add_executable(test_uados_sensors
    test_sensors.cpp
    test_sensor_fusion.cpp
)

target_link_libraries(test_uados_sensors PRIVATE
    uados::sensor_fusion
    uados::sensor_camera
    uados::sensor_lidar
    uados::sensor_radar
    uados::sensor_gps
    uados::sensor_imu
    uados::sensor_api
    GTest::gtest_main
    Eigen3::Eigen
)

gtest_discover_tests(test_uados_sensors)
