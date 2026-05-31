add_executable(test_uados_validation
    test_validation.cpp
)

target_link_libraries(test_uados_validation PRIVATE
    uados::val_automated
    uados::val_fault_injection
    uados::safety_monitors
    GTest::gtest_main
)

gtest_discover_tests(test_uados_validation)
