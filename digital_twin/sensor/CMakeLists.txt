# uados_dtw_sensor
add_library(uados_dtw_sensor STATIC
    src/sensor_twin.cpp
)
add_library(uados::dtw_sensor ALIAS uados_dtw_sensor)

target_include_directories(uados_dtw_sensor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_dtw_sensor PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
