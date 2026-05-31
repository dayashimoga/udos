# uados_loc_visual
add_library(uados_loc_visual INTERFACE)
add_library(uados::loc_visual ALIAS uados_loc_visual)
target_include_directories(uados_loc_visual INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_loc_visual INTERFACE
    uados::common
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
