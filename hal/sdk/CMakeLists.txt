# uados_hal_sdk
add_library(uados_hal_sdk INTERFACE)
add_library(uados::hal_sdk ALIAS uados_hal_sdk)
target_include_directories(uados_hal_sdk INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_hal_sdk INTERFACE
    uados_hal_api
    uados_options
)

