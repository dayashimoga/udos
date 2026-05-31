#include "uados/hal/safety_envelope.hpp"
#include <gtest/gtest.h>

using namespace uados;
using namespace uados::hal;

class SafetyEnvelopeTest : public ::testing::Test {
protected:
    void SetUp() override {
        caps_.max_steering_angle = 0.5; // rad
        caps_.max_speed = 30.0;          // m/s
        caps_.max_acceleration = 3.0;    // m/s^2
        caps_.max_deceleration = 6.0;    // m/s^2
        caps_.wheelbase = 2.8;
        caps_.track_width = 1.6;
        caps_.has_steering = true;
        caps_.has_throttle = true;
        caps_.has_brake = true;
    }

    VehicleCapabilities caps_;
};

TEST_F(SafetyEnvelopeTest, NormalOperationClamping) {
    SafetyEnvelope envelope(caps_);

    VehicleState state{};
    state.velocity = {0.0, 0.0, 0.0}; // Stationary

    VehicleCommand cmd{};
    cmd.throttle = 1.5; // Exceeds limit
    cmd.brake = -0.5;   // Exceeds limit
    cmd.steering_angle = 0.3; // Safe

    auto res = envelope.validate(cmd, state);
    ASSERT_TRUE(res.ok());
    
    auto safe_cmd = *res;
    EXPECT_DOUBLE_EQ(safe_cmd.throttle, 1.0); // Clamped
    EXPECT_DOUBLE_EQ(safe_cmd.brake, 0.0);    // Clamped
    EXPECT_DOUBLE_EQ(safe_cmd.steering_angle, 0.3);
}

TEST_F(SafetyEnvelopeTest, EmergencyStopOverride) {
    SafetyEnvelope envelope(caps_);

    VehicleState state{};
    state.velocity = {10.0, 0.0, 0.0};

    VehicleCommand cmd{};
    cmd.throttle = 0.8;
    cmd.steering_angle = 0.4;
    cmd.emergency_stop = true;

    auto res = envelope.validate(cmd, state);
    ASSERT_TRUE(res.ok());
    
    auto safe_cmd = *res;
    EXPECT_DOUBLE_EQ(safe_cmd.throttle, 0.0); // Zeroed
    EXPECT_DOUBLE_EQ(safe_cmd.brake, 1.0);    // Full brake
    EXPECT_DOUBLE_EQ(safe_cmd.steering_angle, 0.0); // Centered
}

TEST_F(SafetyEnvelopeTest, BrakeOverrideSystem) {
    SafetyEnvelope envelope(caps_);

    VehicleState state{};
    state.velocity = {5.0, 0.0, 0.0};

    VehicleCommand cmd{};
    cmd.throttle = 0.8;
    cmd.brake = 0.5; // Simultaneous application

    auto res = envelope.validate(cmd, state);
    ASSERT_TRUE(res.ok());
    
    auto safe_cmd = *res;
    EXPECT_DOUBLE_EQ(safe_cmd.throttle, 0.0); // Throttle cut
    EXPECT_DOUBLE_EQ(safe_cmd.brake, 0.5);    // Brake preserved
}

TEST_F(SafetyEnvelopeTest, DynamicSteeringRolloverPrevention) {
    SafetyEnvelope envelope(caps_);

    // 1. Test at high speed (e.g. 20m/s = 72 km/h)
    VehicleState state{};
    state.velocity = {20.0, 0.0, 0.0};

    VehicleCommand cmd{};
    cmd.steering_angle = 0.45; // Close to mechanical limit (0.5)

    auto res = envelope.validate(cmd, state);
    ASSERT_TRUE(res.ok());
    
    auto safe_cmd = *res;
    // Max steer limit at 20m/s: 0.5 / (1.0 + 0.01 * 400) = 0.5 / 5.0 = 0.1 rad
    EXPECT_LE(safe_cmd.steering_angle, 0.101);
    EXPECT_GE(safe_cmd.steering_angle, 0.099);
}
