if(UADOS_BUILD_TESTS)
    add_executable(uados_common_tests
        test_types.cpp
        test_version.cpp
        test_hardening.cpp
    )

    target_link_libraries(uados_common_tests PRIVATE
        uados::common
        GTest::gtest_main
    )

    gtest_discover_tests(uados_common_tests)
endif()
