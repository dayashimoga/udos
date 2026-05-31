add_executable(test_uados_scheduler
    test_scheduler.cpp
)

target_link_libraries(test_uados_scheduler PRIVATE
    uados::scheduler
    GTest::gtest_main
)

gtest_discover_tests(test_uados_scheduler)
