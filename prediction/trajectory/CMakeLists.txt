# uados_prediction_trajectory
add_library(uados_prediction_trajectory STATIC
    src/trajectory_predictor.cpp
)
add_library(uados::prediction_trajectory ALIAS uados_prediction_trajectory)

target_include_directories(uados_prediction_trajectory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_prediction_trajectory PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
