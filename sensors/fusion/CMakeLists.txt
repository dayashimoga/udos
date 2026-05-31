# uados_sensor_fusion
add_library(uados_sensor_fusion STATIC
    src/sensor_fusion.cpp
)
add_library(uados::sensor_fusion ALIAS uados_sensor_fusion)

target_include_directories(uados_sensor_fusion PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_sensor_fusion PUBLIC
    uados::sensor_api
    uados_options
    Eigen3::Eigen
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
