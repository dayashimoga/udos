# uados_ctrl_loops
add_library(uados_ctrl_loops STATIC
    src/control_loop.cpp
)
add_library(uados::ctrl_loops ALIAS uados_ctrl_loops)

target_include_directories(uados_ctrl_loops PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_ctrl_loops PUBLIC
    uados::common
    uados_options
    uados::ctrl_steering
    uados::ctrl_throttle
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
