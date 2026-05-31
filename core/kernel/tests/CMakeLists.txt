add_executable(test_uados_kernel
    test_memory_pool.cpp
    test_spsc_queue.cpp
    test_kernel.cpp
)

target_link_libraries(test_uados_kernel PRIVATE
    uados::kernel
    GTest::gtest_main
)

gtest_discover_tests(test_uados_kernel)
