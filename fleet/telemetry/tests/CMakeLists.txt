add_executable(test_uados_fleet
    test_fleet.cpp
)

target_link_libraries(test_uados_fleet PRIVATE
    uados::fleet_telemetry
    uados::fleet_ota
    GTest::gtest_main
)

gtest_discover_tests(test_uados_fleet)
