# uados_sensor_lidar
add_library(uados_sensor_lidar STATIC
    src/lidar_driver.cpp
)
add_library(uados::sensor_lidar ALIAS uados_sensor_lidar)

target_include_directories(uados_sensor_lidar PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sensor_lidar PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
