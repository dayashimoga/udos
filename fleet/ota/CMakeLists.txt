# uados_fleet_ota
add_library(uados_fleet_ota STATIC
    src/ota_manager.cpp
)
add_library(uados::fleet_ota ALIAS uados_fleet_ota)

target_include_directories(uados_fleet_ota PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_fleet_ota PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
