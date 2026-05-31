# uados_sensor_imu
add_library(uados_sensor_imu STATIC
    src/imu_driver.cpp
)
add_library(uados::sensor_imu ALIAS uados_sensor_imu)

target_include_directories(uados_sensor_imu PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sensor_imu PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
