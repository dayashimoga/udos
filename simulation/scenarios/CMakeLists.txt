# uados_sim_scenarios
add_library(uados_sim_scenarios STATIC
    src/scenario_engine.cpp
)
add_library(uados::sim_scenarios ALIAS uados_sim_scenarios)

target_include_directories(uados_sim_scenarios PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sim_scenarios PUBLIC
    uados::common
    uados_options
    uados::dtw_vehicle
    uados::dtw_sensor
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
