# uados_health
add_library(uados_health STATIC
    src/health_monitor_impl.cpp
)
add_library(uados::health ALIAS uados_health)

target_include_directories(uados_health PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_health PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
