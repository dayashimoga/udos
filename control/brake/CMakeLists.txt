# uados_ctrl_brake
add_library(uados_ctrl_brake INTERFACE)
add_library(uados::ctrl_brake ALIAS uados_ctrl_brake)
target_include_directories(uados_ctrl_brake INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_ctrl_brake INTERFACE
    uados::common
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
