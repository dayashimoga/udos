# uados_kernel
add_library(uados_kernel STATIC
    src/config_manager_impl.cpp
    src/kernel_impl.cpp
)
add_library(uados::kernel ALIAS uados_kernel)

target_include_directories(uados_kernel PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_kernel PUBLIC
    uados::common
    uados::event_bus
    uados::scheduler
    uados::health
    uados::lifecycle
    uados::plugin
    uados_options
    yaml-cpp
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
