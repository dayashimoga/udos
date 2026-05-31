# uados_driver_sim
add_library(uados_driver_sim STATIC
    src/carla_driver.cpp
)
add_library(uados::driver_sim ALIAS uados_driver_sim)

target_include_directories(uados_driver_sim PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_driver_sim PUBLIC
    uados::hal_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
