# uados_sensor_radar
add_library(uados_sensor_radar STATIC
    src/radar_driver.cpp
)
add_library(uados::sensor_radar ALIAS uados_sensor_radar)

target_include_directories(uados_sensor_radar PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sensor_radar PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
