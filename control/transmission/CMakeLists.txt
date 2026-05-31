# uados_ctrl_trans
add_library(uados_ctrl_trans INTERFACE)
add_library(uados::ctrl_trans ALIAS uados_ctrl_trans)
target_include_directories(uados_ctrl_trans INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_ctrl_trans INTERFACE
    uados::common
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
