# uados_prediction_risk
add_library(uados_prediction_risk STATIC
    src/risk_estimator.cpp
)
add_library(uados::prediction_risk ALIAS uados_prediction_risk)

target_include_directories(uados_prediction_risk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_prediction_risk PUBLIC
    uados::common
    uados_options
)
