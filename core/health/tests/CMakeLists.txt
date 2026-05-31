add_executable(test_uados_health
    test_health.cpp
)

target_link_libraries(test_uados_health PRIVATE
    uados::health
    GTest::gtest_main
)

gtest_discover_tests(test_uados_health)
