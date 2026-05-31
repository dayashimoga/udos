/// @file test_types.cpp
/// @brief Unit tests for UADOS core type definitions.

#include "uados/types.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace uados::test {

// ============================================================================
// Position3D Tests
// ============================================================================

TEST(Position3DTest, DefaultConstruction) {
    Position3D p;
    EXPECT_DOUBLE_EQ(p.x, 0.0);
    EXPECT_DOUBLE_EQ(p.y, 0.0);
    EXPECT_DOUBLE_EQ(p.z, 0.0);
}

TEST(Position3DTest, ToVec3Conversion) {
    Position3D p{1.0, 2.0, 3.0};
    auto v = p.to_vec3();
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

TEST(Position3DTest, FromVec3Conversion) {
    Vec3 v(4.0, 5.0, 6.0);
    auto p = Position3D::from_vec3(v);
    EXPECT_DOUBLE_EQ(p.x, 4.0);
    EXPECT_DOUBLE_EQ(p.y, 5.0);
    EXPECT_DOUBLE_EQ(p.z, 6.0);
}

// ============================================================================
// Velocity3D Tests
// ============================================================================

TEST(Velocity3DTest, MagnitudeCalculation) {
    Velocity3D v{3.0, 4.0, 0.0};
    EXPECT_DOUBLE_EQ(v.magnitude(), 5.0);
}

TEST(Velocity3DTest, ZeroMagnitude) {
    Velocity3D v{};
    EXPECT_DOUBLE_EQ(v.magnitude(), 0.0);
}

TEST(Velocity3DTest, ThreeDimensionalMagnitude) {
    Velocity3D v{1.0, 2.0, 2.0};
    EXPECT_DOUBLE_EQ(v.magnitude(), 3.0);
}

// ============================================================================
// Acceleration3D Tests
// ============================================================================

TEST(Acceleration3DTest, MagnitudeCalculation) {
    Acceleration3D a{3.0, 4.0, 0.0};
    EXPECT_DOUBLE_EQ(a.magnitude(), 5.0);
}

// ============================================================================
// Pose Tests
// ============================================================================

TEST(PoseTest, IdentityTransformMatrix) {
    Pose p;
    auto m = p.to_matrix();

    // Should be identity matrix
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(m(i, j), 1.0);
            } else {
                EXPECT_NEAR(m(i, j), 0.0, 1e-15);
            }
        }
    }
}

TEST(PoseTest, TranslationInMatrix) {
    Pose p;
    p.position = {10.0, 20.0, 30.0};
    auto m = p.to_matrix();

    EXPECT_DOUBLE_EQ(m(0, 3), 10.0);
    EXPECT_DOUBLE_EQ(m(1, 3), 20.0);
    EXPECT_DOUBLE_EQ(m(2, 3), 30.0);
    EXPECT_DOUBLE_EQ(m(3, 3), 1.0);
}

// ============================================================================
// Status Tests
// ============================================================================

TEST(StatusTest, StatusToString) {
    EXPECT_EQ(status_to_string(Status::Ok), "Ok");
    EXPECT_EQ(status_to_string(Status::Error), "Error");
    EXPECT_EQ(status_to_string(Status::Timeout), "Timeout");
    EXPECT_EQ(status_to_string(Status::NotReady), "NotReady");
    EXPECT_EQ(status_to_string(Status::NotFound), "NotFound");
}

// ============================================================================
// Result Tests
// ============================================================================

TEST(ResultTest, SuccessResult) {
    auto r = Result<int>::success(42);
    EXPECT_TRUE(r.ok());
    EXPECT_EQ(r.status, Status::Ok);
    EXPECT_EQ(*r, 42);
}

TEST(ResultTest, ErrorResult) {
    auto r = Result<int>::error(Status::NotFound, "not found");
    EXPECT_FALSE(r.ok());
    EXPECT_EQ(r.status, Status::NotFound);
    EXPECT_EQ(r.message, "not found");
    EXPECT_FALSE(r.value.has_value());
}

// ============================================================================
// VehicleCommand Tests
// ============================================================================

TEST(VehicleCommandTest, DefaultValues) {
    VehicleCommand cmd;
    EXPECT_DOUBLE_EQ(cmd.steering_angle, 0.0);
    EXPECT_DOUBLE_EQ(cmd.throttle, 0.0);
    EXPECT_DOUBLE_EQ(cmd.brake, 0.0);
    EXPECT_EQ(cmd.gear, GearPosition::Drive);
    EXPECT_FALSE(cmd.emergency_stop);
}

TEST(VehicleCommandTest, EmergencyStopFlag) {
    VehicleCommand cmd;
    cmd.emergency_stop = true;
    EXPECT_TRUE(cmd.emergency_stop);
}

// ============================================================================
// KinematicState Tests
// ============================================================================

TEST(KinematicStateTest, DefaultConstruction) {
    KinematicState state;
    EXPECT_DOUBLE_EQ(state.velocity.magnitude(), 0.0);
    EXPECT_DOUBLE_EQ(state.acceleration.magnitude(), 0.0);
    EXPECT_DOUBLE_EQ(state.yaw_rate, 0.0);
}

// ============================================================================
// Enum Tests
// ============================================================================

TEST(EnumTest, GearPositionValues) {
    EXPECT_NE(GearPosition::Park, GearPosition::Drive);
    EXPECT_NE(GearPosition::Reverse, GearPosition::Neutral);
}

TEST(EnumTest, ObjectClassValues) {
    EXPECT_NE(ObjectClass::Car, ObjectClass::Pedestrian);
    EXPECT_NE(ObjectClass::Unknown, ObjectClass::Bicycle);
}

TEST(EnumTest, TrafficLightStateValues) {
    EXPECT_NE(TrafficLightState::Red, TrafficLightState::Green);
    EXPECT_NE(TrafficLightState::Yellow, TrafficLightState::Off);
}

TEST(EnumTest, SensorTypeValues) {
    EXPECT_NE(SensorType::Camera, SensorType::LiDAR);
    EXPECT_NE(SensorType::Radar, SensorType::GPS);
}

} // namespace uados::test
