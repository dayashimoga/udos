add_executable(test_uados_prediction
    test_prediction.cpp
)

target_link_libraries(test_uados_prediction PRIVATE
    uados::prediction_trajectory
    uados::prediction_behavior
    uados::prediction_risk
    GTest::gtest_main
)

gtest_discover_tests(test_uados_prediction)
