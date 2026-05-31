# uados_sensor_camera
add_library(uados_sensor_camera STATIC
    src/camera_driver.cpp
)
add_library(uados::sensor_camera ALIAS uados_sensor_camera)

target_include_directories(uados_sensor_camera PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sensor_camera PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
