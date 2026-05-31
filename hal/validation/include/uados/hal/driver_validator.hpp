#pragma once

/// @file driver_validator.hpp
/// @brief Automated driver validation harness.

#include "uados/hal/vehicle_driver.hpp"
#include <string>
#include <vector>

namespace uados::hal {

/// @brief Automated test harness for verifying IVehicleDriver implementations.
///
/// Runs standardized compliance tests for driver states, connection transitions,
/// dynamics model telemetry updates, and safety interlocks (Emergency Stop).
class DriverValidator {
public:
    struct TestResult {
        std::string test_name;
        bool passed{false};
        std::string failure_message;
    };

    DriverValidator() = default;
    ~DriverValidator() = default;

    /// Run full validation suite on the given driver
    /// @param driver The driver instance to validate
    /// @return List of test results
    [[nodiscard]] std::vector<TestResult> validate(IVehicleDriver& driver) const;

private:
    [[nodiscard]] TestResult test_lifecycle(IVehicleDriver& driver) const;
    [[nodiscard]] TestResult test_telemetry(IVehicleDriver& driver) const;
    [[nodiscard]] TestResult test_emergency_stop(IVehicleDriver& driver) const;
    [[nodiscard]] TestResult test_steering_clamping(IVehicleDriver& driver) const;
};

} // namespace uados::hal
