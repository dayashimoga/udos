# uados_localization_slam
add_library(uados_localization_slam STATIC
    src/slam_engine.cpp
)
add_library(uados::localization_slam ALIAS uados_localization_slam)

target_include_directories(uados_localization_slam PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_localization_slam PUBLIC
    uados::common
    uados_options
)
