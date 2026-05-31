# uados_ctrl_steering
add_library(uados_ctrl_steering STATIC
    src/stanley_controller.cpp
)
add_library(uados::ctrl_steering ALIAS uados_ctrl_steering)

target_include_directories(uados_ctrl_steering PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_ctrl_steering PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
