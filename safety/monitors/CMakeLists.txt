# uados_safety_monitors
add_library(uados_safety_monitors STATIC
    src/safety_monitor.cpp
)
add_library(uados::safety_monitors ALIAS uados_safety_monitors)
target_include_directories(uados_safety_monitors PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_safety_monitors PUBLIC
    uados::common
    uados_options
    uados::localization_hdmap
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
