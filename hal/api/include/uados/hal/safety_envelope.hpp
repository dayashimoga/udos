#pragma once

/// @file safety_envelope.hpp
/// @brief Actuator limits safety envelope validator.

#include "uados/types.hpp"
#include <string>

namespace uados::hal {

/// @brief Actuator safety envelope validator.
///
/// Ensures all actuator commands (steering, throttle, brake) sent to the physical
/// vehicle or simulation lie within safe mechanical and dynamic envelopes.
/// Includes speed-dependent steering rate-limiting (to prevent high-speed rollover),
/// simultaneous throttle/brake interlocks, and acceleration limits.
class SafetyEnvelope {
public:
    /// Construct with vehicle-specific physical capabilities
    /// @param caps Vehicle physical limits
    explicit SafetyEnvelope(const VehicleCapabilities& caps);

    ~SafetyEnvelope() = default;

    /// Validate and constrain a command based on the current vehicle state
    /// @param cmd The raw input command from control loops
    /// @param state The current real-time vehicle state
    /// @return A safe, rate-limited and clamped command, or an error if unsafe
    [[nodiscard]] Result<VehicleCommand> validate(
        const VehicleCommand& cmd,
        const VehicleState& state) const;

    /// Update capabilities dynamically (e.g., in wet/icy conditions)
    void update_capabilities(const VehicleCapabilities& caps);

    /// Get current active capabilities
    [[nodiscard]] const VehicleCapabilities& capabilities() const noexcept { return caps_; }

private:
    /// Check for illegal command flags or combinations
    [[nodiscard]] Status check_interlocks(const VehicleCommand& cmd) const noexcept;

    /// Limit steering rate based on velocity (prevent rollover)
    [[nodiscard]] Scalar calculate_max_steering_angle(Scalar current_speed) const noexcept;

    VehicleCapabilities caps_;
};

} // namespace uados::hal
