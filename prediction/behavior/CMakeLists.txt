# uados_prediction_behavior
add_library(uados_prediction_behavior STATIC
    src/behavior_predictor.cpp
)
add_library(uados::prediction_behavior ALIAS uados_prediction_behavior)

target_include_directories(uados_prediction_behavior PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_prediction_behavior PUBLIC
    uados::common
    uados_options
)
