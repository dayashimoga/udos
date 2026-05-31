#pragma once

/// @file fault_injector.hpp
/// @brief Fault injection and safety fail-safe recovery validator.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>

namespace uados::validation {

/// @brief Fault injection manager component.
///
/// Injects dynamic anomalies (speed sensor spikes, localization offsets, actuator conflicts)
/// into vehicle state telemetry and command structures to verify safety monitor triggers.
class FaultInjector final : public uados::core::ComponentBase {
public:
    FaultInjector() = default;
    ~FaultInjector() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "validation.fault_injection"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Injects an artificial speed spike anomaly to simulate sensor failure
    /// @param state Target vehicle state feedback to modify
    /// @param speed_spike_mps Added speed discrepancy offset (m/s)
    void inject_speed_spike(VehicleState& state, double speed_spike_mps) const noexcept;

    /// Injects an artificial lateral coordinate shift to simulate localization drift
    /// @param state Target vehicle state feedback to modify
    /// @param lateral_drift_m Shift offset to apply on lateral y axle coordinate (m)
    void inject_lateral_drift(VehicleState& state, double lateral_drift_m) const noexcept;

    /// Injects simultaneous full throttle and brake inputs to evaluate BOS overrides
    /// @param command Target vehicle command actuators to conflict
    void inject_bos_fault(VehicleCommand& command) const noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};
};

} // namespace uados::validation
