# uados_safety_fdi
add_library(uados_safety_fdi INTERFACE)
add_library(uados::safety_fdi ALIAS uados_safety_fdi)
target_include_directories(uados_safety_fdi INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_safety_fdi INTERFACE
    uados::common
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
