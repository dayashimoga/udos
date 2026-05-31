#include "uados/hal/driver_validator.hpp"
#include "uados/logging.hpp"

#include <thread>
#include <chrono>

namespace uados::hal {

UADOS_DECLARE_LOGGER("hal.validation")

std::vector<DriverValidator::TestResult> DriverValidator::validate(IVehicleDriver& driver) const {
    UADOS_LOG_INFO("Running driver validation harness on: {}", driver.name());
    
    std::vector<TestResult> results;
    results.push_back(test_lifecycle(driver));
    results.push_back(test_telemetry(driver));
    results.push_back(test_emergency_stop(driver));
    results.push_back(test_steering_clamping(driver));

    int passed = 0;
    for (const auto& r : results) {
        if (r.passed) {
            passed++;
            UADOS_LOG_INFO("  [PASS] {}", r.test_name);
        } else {
            UADOS_LOG_ERROR("  [FAIL] {}: {}", r.test_name, r.failure_message);
        }
    }
    UADOS_LOG_INFO("Driver Validation complete: {}/{} tests passed.", passed, results.size());

    return results;
}

DriverValidator::TestResult DriverValidator::test_lifecycle(IVehicleDriver& driver) const {
    TestResult result{"Lifecycle State Compliance", false, ""};

    // 1. Check state is Loaded (pre-init) or Initialized (if already initialized)
    auto state = driver.state();
    if (state != ComponentState::Loaded && state != ComponentState::Initialized) {
        result.failure_message = "Initial state must be Loaded or Initialized";
        return result;
    }

    // 2. Initialize
    uados::core::Config config;
    if (state == ComponentState::Loaded) {
        auto status = driver.init(config);
        if (status != Status::Ok) {
            result.failure_message = "init() failed";
            return result;
        }
    }

    if (driver.state() != ComponentState::Initialized) {
        result.failure_message = "State must transition to Initialized after init()";
        return result;
    }

    // 3. Start
    auto status = driver.start();
    if (status != Status::Ok) {
        result.failure_message = "start() failed";
        return result;
    }

    if (driver.state() != ComponentState::Running || !driver.is_connected()) {
        result.failure_message = "State must transition to Running and is_connected must return true after start()";
        return result;
    }

    result.passed = true;
    return result;
}

DriverValidator::TestResult DriverValidator::test_telemetry(IVehicleDriver& driver) const {
    TestResult result{"Telemetry Updates", false, ""};

    if (driver.state() != ComponentState::Running) {
        result.failure_message = "Driver must be in Running state for telemetry testing";
        return result;
    }

    // 1. Initial read
    auto initial_state_res = driver.read_state();
    if (!initial_state_res.ok()) {
        result.failure_message = "Failed to read initial state: " + initial_state_res.message;
        return result;
    }

    // 2. Write commands to accelerate
    VehicleCommand cmd;
    cmd.throttle = 0.5;
    cmd.brake = 0.0;
    cmd.steering_angle = 0.0;
    cmd.gear = GearPosition::Drive;

    auto cmd_status = driver.write_command(cmd);
    if (cmd_status != Status::Ok) {
        result.failure_message = "Failed to write command";
        return result;
    }

    // Wait and read again
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto final_state_res = driver.read_state();
    if (!final_state_res.ok()) {
        result.failure_message = "Failed to read final state: " + final_state_res.message;
        return result;
    }

    auto final_state = *final_state_res;
    if (final_state.velocity.magnitude() <= 0.0 && final_state.acceleration.magnitude() <= 0.0) {
        result.failure_message = "Vehicle did not update speed or acceleration in response to throttle";
        return result;
    }

    result.passed = true;
    return result;
}

DriverValidator::TestResult DriverValidator::test_emergency_stop(IVehicleDriver& driver) const {
    TestResult result{"Emergency Stop Interlock", false, ""};

    if (driver.state() != ComponentState::Running) {
        result.failure_message = "Driver must be in Running state for emergency stop testing";
        return result;
    }

    // Trigger Emergency Stop
    auto status = driver.emergency_stop();
    if (status != Status::Ok) {
        result.failure_message = "emergency_stop() call failed";
        return result;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    auto state_res = driver.read_state();
    if (!state_res.ok()) {
        result.failure_message = "Failed to read state post-ESTOP";
        return result;
    }

    auto state = *state_res;
    if (state.velocity.magnitude() > 0.01) {
        result.failure_message = "Vehicle speed was not zeroed after emergency stop";
        return result;
    }

    result.passed = true;
    return result;
}

DriverValidator::TestResult DriverValidator::test_steering_clamping(IVehicleDriver& driver) const {
    TestResult result{"Steering Clamping and Capabilities check", false, ""};

    auto caps = driver.capabilities();
    if (caps.max_steering_angle <= 0.0) {
        result.failure_message = "Capabilities reported invalid max steering angle";
        return result;
    }

    // Write a steering command that is twice the capability
    VehicleCommand cmd;
    cmd.steering_angle = caps.max_steering_angle * 2.0;
    cmd.throttle = 0.0;
    cmd.brake = 0.0;

    auto cmd_status = driver.write_command(cmd);
    if (cmd_status != Status::Ok) {
        result.failure_message = "Command write failed";
        return result;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    auto state_res = driver.read_state();
    if (!state_res.ok()) {
        result.failure_message = "Failed to read state";
        return result;
    }

    auto state = *state_res;
    if (std::abs(state.steering_angle) > caps.max_steering_angle + 0.001) {
        result.failure_message = "Steering angle feedback exceeded mechanical capabilities limits";
        return result;
    }

    result.passed = true;
    return result;
}

} // namespace uados::hal
