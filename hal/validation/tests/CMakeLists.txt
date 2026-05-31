add_executable(test_uados_hal
    test_safety_envelope.cpp
    test_driver_validation.cpp
)

target_link_libraries(test_uados_hal PRIVATE
    uados::hal_validation
    uados::driver_sim
    uados::driver_rc_car
    uados::driver_can
    uados::hal_api
    GTest::gtest_main
)

gtest_discover_tests(test_uados_hal)
