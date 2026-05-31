add_executable(test_uados_localization
    test_localization.cpp
)

target_link_libraries(test_uados_localization PRIVATE
    uados::localization_pose
    uados::localization_hdmap
    uados::localization_slam
    GTest::gtest_main
)

gtest_discover_tests(test_uados_localization)
