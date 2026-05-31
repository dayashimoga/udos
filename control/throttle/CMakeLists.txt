# uados_ctrl_throttle
add_library(uados_ctrl_throttle STATIC
    src/longitudinal_controller.cpp
)
add_library(uados::ctrl_throttle ALIAS uados_ctrl_throttle)

target_include_directories(uados_ctrl_throttle PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_ctrl_throttle PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
