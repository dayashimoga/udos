# uados_loc_gps
add_library(uados_loc_gps INTERFACE)
add_library(uados::loc_gps ALIAS uados_loc_gps)
target_include_directories(uados_loc_gps INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_loc_gps INTERFACE
    uados::common
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
