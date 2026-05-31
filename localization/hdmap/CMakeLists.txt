# uados_localization_hdmap
add_library(uados_localization_hdmap STATIC
    src/hdmap_engine.cpp
)
add_library(uados::localization_hdmap ALIAS uados_localization_hdmap)

target_include_directories(uados_localization_hdmap PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_localization_hdmap PUBLIC
    uados::common
    uados_options
)
