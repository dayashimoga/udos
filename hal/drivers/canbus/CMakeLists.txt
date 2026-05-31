# uados_driver_can
add_library(uados_driver_can STATIC
    src/canbus_driver.cpp
)
add_library(uados::driver_can ALIAS uados_driver_can)

target_include_directories(uados_driver_can PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_driver_can PUBLIC
    uados::hal_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
