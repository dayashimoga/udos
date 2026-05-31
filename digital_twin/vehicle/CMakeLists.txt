# uados_dtw_vehicle
add_library(uados_dtw_vehicle STATIC
    src/vehicle_twin.cpp
)
add_library(uados::dtw_vehicle ALIAS uados_dtw_vehicle)

target_include_directories(uados_dtw_vehicle PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_dtw_vehicle PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
