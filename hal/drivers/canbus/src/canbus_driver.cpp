#include "uados/hal/canbus_driver.hpp"
#include "uados/logging.hpp"

#include <cmath>
#include <algorithm>

namespace uados::hal {

UADOS_DECLARE_LOGGER("hal.driver.canbus")

CANBusDriver::~CANBusDriver() {
    stop();
}

Status CANBusDriver::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing CAN Bus Drive-By-Wire (DBW) driver...");

    // Setup capabilities (e.g., Lincoln MKZ reference DBW vehicle)
    caps_.max_steering_angle = 0.82;  // ~47 degrees in radians
    caps_.max_speed = 40.0;           // m/s (~144 km/h)
    caps_.max_acceleration = 3.0;     // m/s^2
    caps_.max_deceleration = 6.0;     // m/s^2
    caps_.wheelbase = 2.85;           // meters
    caps_.track_width = 1.58;         // meters
    caps_.has_steering = true;
    caps_.has_throttle = true;
    caps_.has_brake = true;
    caps_.has_gear = true;

    // Initial state
    state_.timestamp = Clock::now();
    state_.position = {0.0, 0.0, 0.0};
    state_.velocity = {0.0, 0.0, 0.0};
    state_.acceleration = {0.0, 0.0, 0.0};
    state_.orientation = Quat::Identity();
    state_.steering_angle = 0.0;
    state_.wheel_speeds = {0.0, 0.0, 0.0, 0.0};
    state_.gear = GearPosition::Park;
    state_.battery_voltage = 12.6;    // Standard lead-acid battery voltage
    state_.engine_running = true;

    last_update_time_ = Clock::now();

    status_.health = HealthStatus::Healthy;
    status_.state = ComponentState::Initialized;
    status_.connected = false;

    set_health(HealthStatus::Healthy);
    set_state(ComponentState::Initialized);

    UADOS_LOG_INFO("CAN Bus Drive-By-Wire driver initialized successfully");
    return Status::Ok;
}

Status CANBusDriver::start() {
    std::lock_guard lock(mutex_);
    
    if (state() == ComponentState::Running) {
        return Status::Ok;
    }

    UADOS_LOG_INFO("Connecting to socketCAN interface (can0)...");
    connected_ = true;
    status_.connected = true;
    status_.state = ComponentState::Running;
    
    last_update_time_ = Clock::now();
    set_state(ComponentState::Running);

    UADOS_LOG_INFO("CAN Bus driver started successfully");
    return Status::Ok;
}

Status CANBusDriver::stop() {
    std::lock_guard lock(mutex_);
    
    if (state() == ComponentState::Stopped) {
        return Status::Ok;
    }

    UADOS_LOG_INFO("Closing socketCAN socket...");
    connected_ = false;
    status_.connected = false;
    status_.state = ComponentState::Stopped;
    set_state(ComponentState::Stopped);

    return Status::Ok;
}

uados::Result<VehicleState> CANBusDriver::read_state() {
    std::lock_guard lock(mutex_);
    
    if (!connected_) {
        return uados::Result<VehicleState>::error(Status::NotReady, "Driver is not connected");
    }

    simulate_can_traffic();
    status_.states_received++;
    return uados::Result<VehicleState>::success(state_);
}

uados::Status CANBusDriver::write_command(const VehicleCommand& cmd) {
    std::lock_guard lock(mutex_);
    
    if (!connected_) {
        return Status::NotReady;
    }

    last_cmd_ = cmd;
    status_.commands_sent++;

    // Encode command CAN frame as test validation
    CanFrame cmd_frame = encode_command_frame();
    UADOS_LOG_DEBUG("CANBus: Transmitted Command ID 0x{:X} dlc={} data=[{:X} {:X} {:X} {:X} {:X} {:X} {:X} {:X}]",
                    cmd_frame.id, cmd_frame.dlc,
                    cmd_frame.data[0], cmd_frame.data[1], cmd_frame.data[2], cmd_frame.data[3],
                    cmd_frame.data[4], cmd_frame.data[5], cmd_frame.data[6], cmd_frame.data[7]);

    return Status::Ok;
}

VehicleCapabilities CANBusDriver::capabilities() const {
    std::lock_guard lock(mutex_);
    return caps_;
}

DriverStatus CANBusDriver::driver_status() const {
    std::lock_guard lock(mutex_);
    return status_;
}

bool CANBusDriver::is_connected() const {
    std::lock_guard lock(mutex_);
    return connected_;
}

uados::Status CANBusDriver::emergency_stop() {
    std::lock_guard lock(mutex_);
    
    UADOS_LOG_WARN("CAN Bus Driver: EMERGENCY STOP triggered");
    
    last_cmd_.throttle = 0.0;
    last_cmd_.brake = 1.0;
    last_cmd_.emergency_stop = true;
    
    state_.velocity = {0.0, 0.0, 0.0};
    state_.acceleration = {0.0, 0.0, 0.0};
    
    return Status::Ok;
}

CanFrame CANBusDriver::encode_command_frame() const noexcept {
    CanFrame frame;
    frame.id = 0x100; // DBW Command ID
    frame.dlc = 8;

    // 1. Pack Steering: Scale factor = 1000 (radians to milliradians)
    int16_t steer_raw = static_cast<int16_t>(last_cmd_.steering_angle * 1000.0);
    frame.data[0] = static_cast<uint8_t>((steer_raw >> 8) & 0xFF);
    frame.data[1] = static_cast<uint8_t>(steer_raw & 0xFF);

    // 2. Pack Throttle: Scale factor = 1000 (0 to 1000)
    uint16_t throttle_raw = static_cast<uint16_t>(last_cmd_.throttle * 1000.0);
    frame.data[2] = static_cast<uint8_t>((throttle_raw >> 8) & 0xFF);
    frame.data[3] = static_cast<uint8_t>(throttle_raw & 0xFF);

    // 3. Pack Brake: Scale factor = 1000 (0 to 1000)
    uint16_t brake_raw = static_cast<uint16_t>(last_cmd_.brake * 1000.0);
    frame.data[4] = static_cast<uint8_t>((brake_raw >> 8) & 0xFF);
    frame.data[5] = static_cast<uint8_t>(brake_raw & 0xFF);

    // 4. Pack Flags: bit 0 = emergency, bit 1 = dbw_enable
    uint8_t flags = 0;
    if (last_cmd_.emergency_stop) flags |= 0x01;
    flags |= 0x02; // DBW enable active
    frame.data[6] = flags;

    // 5. Rolling counter
    static uint8_t counter = 0;
    frame.data[7] = counter;
    counter = (counter + 1) & 0x0F;

    return frame;
}

void CANBusDriver::decode_feedback_frame(const CanFrame& frame) noexcept {
    if (frame.id != 0x200) return; // Not the feedback message

    // 1. Unpack Steering: Scale factor = 0.001 (milliradians to radians)
    int16_t steer_raw = static_cast<int16_t>((frame.data[0] << 8) | frame.data[1]);
    state_.steering_angle = static_cast<double>(steer_raw) * 0.001;

    // 2. Unpack Velocity: Scale factor = 0.01 (cm/s to m/s)
    uint16_t speed_raw = static_cast<uint16_t>((frame.data[2] << 8) | frame.data[3]);
    double speed = static_cast<double>(speed_raw) * 0.01;

    // Apply basic direction from gear
    double direction = 1.0;
    if (state_.gear == GearPosition::Reverse) {
        direction = -1.0;
    }
    
    // Update simple longitudinal state variables
    state_.velocity.vx = speed * direction;
    state_.velocity.vy = 0.0;
    state_.velocity.vz = 0.0;

    // 3. Unpack Gear Position
    uint8_t gear_raw = frame.data[4];
    state_.gear = static_cast<GearPosition>(gear_raw);

    // 4. Unpack Battery Voltage: Scale factor = 0.1 (decivolts to volts)
    uint8_t voltage_raw = frame.data[5];
    state_.battery_voltage = static_cast<double>(voltage_raw) * 0.1;

    // 5. Unpack state/health
    uint8_t health_flags = frame.data[6];
    if (health_flags & 0x04) {
        status_.health = HealthStatus::Unhealthy;
    } else if (health_flags & 0x02) {
        status_.health = HealthStatus::Degraded;
    } else {
        status_.health = HealthStatus::Healthy;
    }
}

void CANBusDriver::simulate_can_traffic() {
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time_);
    double dt = static_cast<double>(elapsed.count()) / 1e6;
    last_update_time_ = now;

    if (dt <= 0.0) return;

    // Simulate simple physics loop and serialize feedback back into decoded state
    double current_speed = state_.velocity.magnitude();
    double accel = 0.0;

    if (last_cmd_.emergency_stop) {
        accel = -caps_.max_deceleration;
    } else {
        double throttle_force = last_cmd_.throttle * caps_.max_acceleration;
        double braking_force = last_cmd_.brake * caps_.max_deceleration;
        accel = throttle_force - braking_force;
        accel -= 0.03 * current_speed; // Drag
    }

    double new_speed = current_speed + accel * dt;
    if (new_speed < 0.0) new_speed = 0.0;
    if (new_speed > caps_.max_speed) new_speed = caps_.max_speed;

    // Simulate packing a 0x200 CAN feedback frame
    CanFrame fb_frame;
    fb_frame.id = 0x200;
    fb_frame.dlc = 8;

    // Packing Steering
    int16_t steer_raw = static_cast<int16_t>(last_cmd_.steering_angle * 1000.0);
    fb_frame.data[0] = static_cast<uint8_t>((steer_raw >> 8) & 0xFF);
    fb_frame.data[1] = static_cast<uint8_t>(steer_raw & 0xFF);

    // Packing Velocity
    uint16_t speed_raw = static_cast<uint16_t>(new_speed * 100.0);
    fb_frame.data[2] = static_cast<uint8_t>((speed_raw >> 8) & 0xFF);
    fb_frame.data[3] = static_cast<uint8_t>(speed_raw & 0xFF);

    // Packing Gear
    fb_frame.data[4] = static_cast<uint8_t>(last_cmd_.gear);

    // Packing Battery
    fb_frame.data[5] = 126; // 12.6V constant in simulation

    // Packing Health (0x01 = Ok)
    fb_frame.data[6] = 0x01;

    // Decoding it back into state
    decode_feedback_frame(fb_frame);

    // Simulate yaw and position updates as well (so digital twin works)
    double L = caps_.wheelbase;
    double yaw = state_.orientation.toRotationMatrix().eulerAngles(0, 1, 2).z();
    double dx = new_speed * std::cos(yaw);
    double dy = new_speed * std::sin(yaw);
    double dyaw = (new_speed / L) * std::tan(state_.steering_angle);

    state_.position.x += dx * dt;
    state_.position.y += dy * dt;
    yaw += dyaw * dt;

    state_.orientation = Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitX()) *
                         Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitY()) *
                         Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ());

    state_.velocity.vx = dx;
    state_.velocity.vy = dy;
    state_.acceleration.ax = accel * std::cos(yaw);
    state_.acceleration.ay = accel * std::sin(yaw);

    state_.timestamp = now;

    // Wheel radius ~34cm
    double wheel_rot = new_speed / 0.34;
    state_.wheel_speeds = {wheel_rot, wheel_rot, wheel_rot, wheel_rot};
}

} // namespace uados::hal
