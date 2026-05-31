# uados_driver_rc_car
add_library(uados_driver_rc_car STATIC
    src/rc_car_driver.cpp
)
add_library(uados::driver_rc_car ALIAS uados_driver_rc_car)

target_include_directories(uados_driver_rc_car PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_driver_rc_car PUBLIC
    uados::hal_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
