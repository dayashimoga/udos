# uados_sensor_gps
add_library(uados_sensor_gps STATIC
    src/gps_driver.cpp
)
add_library(uados::sensor_gps ALIAS uados_sensor_gps)

target_include_directories(uados_sensor_gps PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sensor_gps PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
