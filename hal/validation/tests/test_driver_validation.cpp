#include "uados/hal/carla_driver.hpp"
#include "uados/hal/rc_car_driver.hpp"
#include "uados/hal/canbus_driver.hpp"
#include "uados/hal/driver_validator.hpp"
#include <gtest/gtest.h>

using namespace uados;
using namespace uados::hal;

TEST(DriverValidationTest, CARLADriverCompliance) {
    CARLADriver driver;
    DriverValidator validator;

    // Run automated validation
    auto results = validator.validate(driver);
    ASSERT_EQ(results.size(), 4);
    
    for (const auto& r : results) {
        EXPECT_TRUE(r.passed) << "Failed test: " << r.test_name << " - " << r.failure_message;
    }
}

TEST(DriverValidationTest, RCCarDriverCompliance) {
    RCCarDriver driver;
    DriverValidator validator;

    auto results = validator.validate(driver);
    ASSERT_EQ(results.size(), 4);
    
    for (const auto& r : results) {
        EXPECT_TRUE(r.passed) << "Failed test: " << r.test_name << " - " << r.failure_message;
    }

    // Additional RC Car PWM translation tests
    uados::core::Config config;
    driver.init(config);
    driver.start();

    VehicleCommand cmd;
    cmd.steering_angle = driver.capabilities().max_steering_angle; // Full right
    cmd.throttle = 0.5;
    cmd.brake = 0.0;
    driver.write_command(cmd);

    // Verify PWM translates correctly (Steering -> 2000us, Throttle -> 1750us)
    EXPECT_EQ(driver.get_steering_pwm(), 2000);
    EXPECT_EQ(driver.get_throttle_pwm(), 1750);

    driver.stop();
}

TEST(DriverValidationTest, CANBusDriverCompliance) {
    CANBusDriver driver;
    DriverValidator validator;

    auto results = validator.validate(driver);
    ASSERT_EQ(results.size(), 4);
    
    for (const auto& r : results) {
        EXPECT_TRUE(r.passed) << "Failed test: " << r.test_name << " - " << r.failure_message;
    }

    // Additional CAN frame bit-packing verification
    uados::core::Config config;
    driver.init(config);
    driver.start();

    VehicleCommand cmd;
    cmd.steering_angle = 0.25; // 0.25 rad = 250 milliradians
    cmd.throttle = 0.8;        // 80% throttle = 800 scale
    cmd.brake = 0.0;
    cmd.emergency_stop = false;
    driver.write_command(cmd);

    CanFrame frame = driver.encode_command_frame();
    EXPECT_EQ(frame.id, 0x100);

    // Verify unpacked steering (data[0-1] = 250 = 0x00FA)
    int16_t unpacked_steer = (frame.data[0] << 8) | frame.data[1];
    EXPECT_EQ(unpacked_steer, 250);

    // Verify unpacked throttle (data[2-3] = 800 = 0x0320)
    uint16_t unpacked_throttle = (frame.data[2] << 8) | frame.data[3];
    EXPECT_EQ(unpacked_throttle, 800);

    driver.stop();
}
