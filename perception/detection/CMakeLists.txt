# uados_perception_detection
add_library(uados_perception_detection STATIC
    src/inference_engine.cpp
    src/object_detector.cpp
)
add_library(uados::perception_detection ALIAS uados_perception_detection)

target_include_directories(uados_perception_detection PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_perception_detection PUBLIC
    uados::sensor_api
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
