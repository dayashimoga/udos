# uados_fleet_telemetry
add_library(uados_fleet_telemetry STATIC
    src/fleet_telemetry.cpp
)
add_library(uados::fleet_telemetry ALIAS uados_fleet_telemetry)

target_include_directories(uados_fleet_telemetry PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_fleet_telemetry PUBLIC
    uados::common
    uados_options
    nlohmann_json::nlohmann_json
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
