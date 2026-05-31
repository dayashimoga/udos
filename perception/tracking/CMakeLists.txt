# uados_perception_tracking
add_library(uados_perception_tracking STATIC
    src/object_tracker.cpp
)
add_library(uados::perception_tracking ALIAS uados_perception_tracking)

target_include_directories(uados_perception_tracking PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_perception_tracking PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
