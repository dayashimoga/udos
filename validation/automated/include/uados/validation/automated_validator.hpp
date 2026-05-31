#pragma once

/// @file automated_validator.hpp
/// @brief Automated compliance test validator and evidence reporter.

#include "uados/component.hpp"
#include "uados/types.hpp"
#include "uados/simulation/scenario_engine.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::validation {

/// @brief Struct representing a compliance test case outcome
struct TestCaseResult {
    std::string name;
    bool passed{false};
    double max_cross_track_error{0.0};
    double min_obstacle_distance{999.0};
    std::string details;
};

/// @brief Automated Compliance Validator component.
///
/// Runs virtual batch scenario suites (nominal cruising, stop lines, yields, emergencies)
/// and compiles structured JSON/Markdown validation evidence report logs.
class AutomatedValidator final : public uados::core::ComponentBase {
public:
    AutomatedValidator() = default;
    ~AutomatedValidator() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "validation.automated"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Executes the entire compliance test suite in batch
    void run_validation_suite() noexcept;

    /// Compiles a beautifully formatted Markdown compliance report
    [[nodiscard]] std::string compile_evidence_report() const noexcept;

    /// Returns the cached results of the latest test run
    [[nodiscard]] std::vector<TestCaseResult> get_results() const noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    std::vector<TestCaseResult> results_;
    uados::simulation::ScenarioEngine scenario_engine_;
};

} // namespace uados::validation
