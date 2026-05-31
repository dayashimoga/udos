add_executable(test_uados_simulation
    test_simulation.cpp
)

target_link_libraries(test_uados_simulation PRIVATE
    uados::sim_scenarios
    uados::sim_replay
    GTest::gtest_main
)

gtest_discover_tests(test_uados_simulation)
