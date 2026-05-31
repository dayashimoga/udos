#pragma once

/// @file emergency_response_system.hpp
/// @brief Emergency response and Minimum Risk Condition (MRC) executor.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>

namespace uados::safety {

/// @brief Emergency System state
enum class EmergencyState : uint8_t {
    Normal,         ///< Nominal autonomous operations
    ActiveMRC,      ///< Actively executing safe deceleration MRC profile
    SafeState       ///< Vehicle at full stop in Park gear
};

/// @brief Converts EmergencyState to string
[[nodiscard]] constexpr std::string_view emergency_state_to_string(EmergencyState s) noexcept {
    switch (s) {
        case EmergencyState::Normal:    return "Normal";
        case EmergencyState::ActiveMRC: return "ActiveMRC";
        case EmergencyState::SafeState: return "SafeState";
    }
    return "Unknown";
}

/// @brief Emergency Response System component.
///
/// Manages the state transition to the Minimum Risk Condition (MRC),
/// overriding command targets to decelerate the vehicle to a full stop and lock in Park.
class EmergencyResponseSystem final : public uados::core::ComponentBase {
public:
    EmergencyResponseSystem() = default;
    ~EmergencyResponseSystem() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "safety.emergency"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Triggers the transition into Minimum Risk Condition
    void trigger_mrc() noexcept;

    /// Resets the FSM back to nominal state (e.g. after manual recovery)
    void reset_nominal() noexcept;

    /// Executes the emergency override command signals
    /// @param state Current vehicle kinematic speed and gear feedback
    /// @param command Nominal vehicle command to override with safety overrides
    /// @param dt Loop time delta (s)
    void execute_mrc(const VehicleState& state, VehicleCommand& command, double dt) noexcept;

    /// Query the current emergency FSM state
    [[nodiscard]] EmergencyState get_emergency_state() const noexcept { return state_; }

private:
    mutable std::mutex mutex_;
    bool active_{false};

    EmergencyState state_{EmergencyState::Normal};

    double mrc_deceleration_{3.0}; ///< Target deceleration rate during MRC (m/s²)
    double time_in_mrc_{0.0};
};

} // namespace uados::safety
