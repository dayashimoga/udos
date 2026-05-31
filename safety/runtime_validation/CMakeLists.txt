# uados_safety_rv
add_library(uados_safety_rv INTERFACE)
add_library(uados::safety_rv ALIAS uados_safety_rv)
target_include_directories(uados_safety_rv INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_safety_rv INTERFACE
    uados::common
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
