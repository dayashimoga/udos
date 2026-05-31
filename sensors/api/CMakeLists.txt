# uados_sensor_api
add_library(uados_sensor_api INTERFACE)
add_library(uados::sensor_api ALIAS uados_sensor_api)
target_include_directories(uados_sensor_api INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_sensor_api INTERFACE
    uados::common
    uados_options
)

