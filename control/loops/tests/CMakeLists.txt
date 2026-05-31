add_executable(test_uados_control
    test_control.cpp
)

target_link_libraries(test_uados_control PRIVATE
    uados::ctrl_loops
    uados::ctrl_steering
    uados::ctrl_throttle
    GTest::gtest_main
)

gtest_discover_tests(test_uados_control)
