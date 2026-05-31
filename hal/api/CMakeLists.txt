# uados_hal_api
add_library(uados_hal_api STATIC
    src/safety_envelope.cpp
)
add_library(uados::hal_api ALIAS uados_hal_api)

target_include_directories(uados_hal_api PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_hal_api PUBLIC
    uados::common
    uados_options
)
