add_executable(test_uados_planning
    test_planning.cpp
)

target_link_libraries(test_uados_planning PRIVATE
    uados::plan_strategic
    uados::plan_behavior
    uados::plan_motion
    uados::localization_hdmap
    GTest::gtest_main
)

gtest_discover_tests(test_uados_planning)
