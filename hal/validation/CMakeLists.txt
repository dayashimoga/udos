# uados_hal_validation
add_library(uados_hal_validation STATIC
    src/driver_validator.cpp
)
add_library(uados::hal_validation ALIAS uados_hal_validation)

target_include_directories(uados_hal_validation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_hal_validation PUBLIC
    uados::hal_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
