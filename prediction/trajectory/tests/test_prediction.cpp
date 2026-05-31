#include "uados/prediction/trajectory_predictor.hpp"
#include "uados/prediction/behavior_predictor.hpp"
#include "uados/prediction/risk_estimator.hpp"
#include <gtest/gtest.h>

using namespace uados;
using namespace uados::prediction;

TEST(PredictionTest, TrajectoryPredictorConstantVelocity) {
    TrajectoryPredictor predictor;
    uados::core::Config config;

    ASSERT_EQ(predictor.init(config), Status::Ok);
    ASSERT_EQ(predictor.start(), Status::Ok);

    // 1. Create a tracked obstacle moving East at 10 m/s
    DetectedObject obj;
    obj.id = 101;
    obj.object_class = ObjectClass::Car;
    obj.position = {10.0, 0.0, 0.0};
    obj.velocity = {10.0, 0.0, 0.0};
    obj.acceleration = {0.0, 0.0, 0.0};

    auto predictions = predictor.predict({obj}, 2.0, 0.5); // 2s horizon, 0.5s steps -> 4 points
    ASSERT_EQ(predictions.size(), 1);
    EXPECT_EQ(predictions[0].obstacle_id, 101);
    
    auto path = predictions[0].paths[0];
    ASSERT_EQ(path.points.size(), 4);

    // Point 1: t=0.5s -> x = 10 + 10 * 0.5 = 15m
    EXPECT_NEAR(path.points[0].position.x, 15.0, 0.01);
    EXPECT_DOUBLE_EQ(path.points[0].position.y, 0.0);

    // Point 4: t=2.0s -> x = 10 + 10 * 2.0 = 30m
    EXPECT_NEAR(path.points[3].position.x, 30.0, 0.01);

    predictor.stop();
}

TEST(PredictionTest, BehaviorPredictorClassification) {
    BehaviorPredictor predictor;
    uados::core::Config config;

    ASSERT_EQ(predictor.init(config), Status::Ok);
    ASSERT_EQ(predictor.start(), Status::Ok);

    // 1. Cruising car
    DetectedObject c1;
    c1.id = 1;
    c1.velocity = {10.0, 0.0, 0.0};
    c1.acceleration = {0.0, 0.0, 0.0};

    // 2. Lane changer left (vy > 0.35)
    DetectedObject c2;
    c2.id = 2;
    c2.velocity = {10.0, 0.5, 0.0};
    c2.acceleration = {0.0, 0.0, 0.0};

    // 3. Braking car (ax < -0.8)
    DetectedObject c3;
    c3.id = 3;
    c3.velocity = {10.0, 0.0, 0.0};
    c3.acceleration = {-1.5, 0.0, 0.0};

    auto behaviors = predictor.predict_intentions({c1, c2, c3});
    ASSERT_EQ(behaviors.size(), 3);

    EXPECT_EQ(behaviors[0].primary_intention, Intention::Cruising);
    EXPECT_EQ(behaviors[1].primary_intention, Intention::LaneChangingLeft);
    EXPECT_EQ(behaviors[2].primary_intention, Intention::Braking);

    predictor.stop();
}

TEST(PredictionTest, RiskEstimatorTtcWarnings) {
    RiskEstimator estimator;
    uados::core::Config config;

    ASSERT_EQ(estimator.init(config), Status::Ok);
    ASSERT_EQ(estimator.start(), Status::Ok);

    // Ego car moving at 15m/s (54 km/h)
    VehicleState ego;
    ego.velocity = {15.0, 0.0, 0.0};

    // 1. Stationary car directly ahead (dx = 15m, speed = 0)
    // Relative speed closing rate = 15m/s -> TTC = 15m / 15m/s = 1.0s (Critical!)
    DetectedObject o1;
    o1.id = 10;
    o1.position = {15.0, 0.0, 0.0}; // directly in front (dy = 0)
    o1.velocity = {0.0, 0.0, 0.0};

    // 2. Obstacle far away (dx = 60m, speed = 0) -> TTC = 60 / 15 = 4.0s (Low risk)
    DetectedObject o2;
    o2.id = 20;
    o2.position = {60.0, 0.0, 0.0};
    o2.velocity = {0.0, 0.0, 0.0};

    // 3. Obstacle off path (dx = 15m, dy = 3.0m left) -> outside tunnel, Low risk
    DetectedObject o3;
    o3.id = 30;
    o3.position = {15.0, 3.0, 0.0};
    o3.velocity = {0.0, 0.0, 0.0};

    auto risks = estimator.estimate_risks(ego, {o1, o2, o3});
    ASSERT_EQ(risks.size(), 3);

    EXPECT_EQ(risks[0].threat_level, ThreatLevel::Critical);
    EXPECT_NEAR(risks[0].time_to_collision, 1.0, 0.01);

    EXPECT_EQ(risks[1].threat_level, ThreatLevel::Low);
    EXPECT_EQ(risks[2].threat_level, ThreatLevel::Low);

    estimator.stop();
}
