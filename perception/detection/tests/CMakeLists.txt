add_executable(test_uados_perception
    test_perception.cpp
)

target_link_libraries(test_uados_perception PRIVATE
    uados::perception_detection
    uados::perception_tracking
    uados::perception_lanes
    uados::perception_traffic_lights
    uados::sensor_api
    GTest::gtest_main
)

gtest_discover_tests(test_uados_perception)
