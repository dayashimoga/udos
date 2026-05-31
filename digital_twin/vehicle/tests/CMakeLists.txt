add_executable(test_uados_digital_twin
    test_digital_twin.cpp
)

target_link_libraries(test_uados_digital_twin PRIVATE
    uados::dtw_vehicle
    uados::dtw_sensor
    GTest::gtest_main
)

gtest_discover_tests(test_uados_digital_twin)
