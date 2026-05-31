# uados_perception_traffic_lights
add_library(uados_perception_traffic_lights STATIC
    src/traffic_light_detector.cpp
)
add_library(uados::perception_traffic_lights ALIAS uados_perception_traffic_lights)

target_include_directories(uados_perception_traffic_lights PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_perception_traffic_lights PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
