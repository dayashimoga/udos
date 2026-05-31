# uados_perception_lanes
add_library(uados_perception_lanes STATIC
    src/lane_detector.cpp
)
add_library(uados::perception_lanes ALIAS uados_perception_lanes)

target_include_directories(uados_perception_lanes PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_perception_lanes PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
