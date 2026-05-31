#pragma once

/// @file safety_monitor.hpp
/// @brief Runtime safety invariant monitor.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/localization/hdmap_engine.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::safety {

/// @brief Struct representing a logged safety violation detail
struct SafetyViolation {
    std::string rule_name;
    SafetySeverity severity{SafetySeverity::Info};
    std::string description;
    Timestamp timestamp{};
};

/// @brief Safety Monitor component.
///
/// Actively audits vehicle states and actuator commands in real-time,
/// enforcing speed, boundary, steer-rate, and Brake Override (BOS) interlocks.
class SafetyMonitor final : public uados::core::ComponentBase {
public:
    SafetyMonitor() = default;
    ~SafetyMonitor() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "safety.monitors"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Audits vehicle commands and state parameters against safety invariants
    /// @param state Current kinematic state feedback from vehicle
    /// @param current_lanelet_info HD Map road segment parameters
    /// @param lateral_error Lateral cross-track deviation of vehicle (m)
    /// @param heading_error Heading tracking error of vehicle (rad)
    /// @param command Desired actuator command (may be modified/clamped for safety)
    /// @return Status::Ok if invariants hold, Status::Error if critical breach occurs
    [[nodiscard]] Status audit_safety(
        const VehicleState& state,
        const uados::localization::LaneletInfo& current_lanelet_info,
        double lateral_error,
        double heading_error,
        VehicleCommand& command);

    /// Query the list of recently caught safety violations
    [[nodiscard]] std::vector<SafetyViolation> get_violations() const noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    std::vector<SafetyViolation> violations_;

    // Safety margins (configurable via YAML)
    double speed_buffer_mps_{2.0};       ///< Speed limit buffer (m/s)
    double max_lateral_error_limit_{1.8};///< Absolute ODD lateral error boundary (m)
    double max_heading_error_limit_{0.8};///< Absolute ODD heading error boundary (rad)
    double max_steering_limit_{0.72};    ///< Mechanical steering saturation guard (rad)

    void record_violation(const std::string& rule, SafetySeverity severity, const std::string& desc) noexcept;
};

} // namespace uados::safety
