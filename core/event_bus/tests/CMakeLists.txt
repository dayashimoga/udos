add_executable(test_uados_event_bus
    test_event_bus.cpp
)

target_link_libraries(test_uados_event_bus PRIVATE
    uados::event_bus
    GTest::gtest_main
)

gtest_discover_tests(test_uados_event_bus)
