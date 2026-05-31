# uados_sim_replay
add_library(uados_sim_replay STATIC
    src/replay_system.cpp
)
add_library(uados::sim_replay ALIAS uados_sim_replay)

target_include_directories(uados_sim_replay PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sim_replay PUBLIC
    uados::common
    uados_options
    nlohmann_json::nlohmann_json
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
