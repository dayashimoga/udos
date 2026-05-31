add_executable(test_uados_safety
    test_safety.cpp
)

target_link_libraries(test_uados_safety PRIVATE
    uados::safety_monitors
    uados::safety_emergency
    uados::localization_hdmap
    GTest::gtest_main
)

gtest_discover_tests(test_uados_safety)
