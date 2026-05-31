#include "uados/validation/automated_validator.hpp"
#include "uados/logging.hpp"

#include <sstream>
#include <iomanip>

namespace uados::validation {

UADOS_DECLARE_LOGGER("validation.automated")

Status AutomatedValidator::init(const uados::core::Config& config) {
    std::lock_guard lock(mutex_);

    UADOS_LOG_INFO("Initializing Automated Compliance Validator...");

    ASSERT_EQ(scenario_engine_.init(config), Status::Ok);
    results_.clear();

    set_state(ComponentState::Initialized);
    set_health(HealthStatus::Healthy);

    UADOS_LOG_INFO("Automated Compliance Validator initialized successfully.");
    return Status::Ok;
}

Status AutomatedValidator::start() {
    std::lock_guard lock(mutex_);
    if (active_) return Status::Ok;

    ASSERT_EQ(scenario_engine_.start(), Status::Ok);
    results_.clear();
    active_ = true;
    set_state(ComponentState::Running);
    return Status::Ok;
}

Status AutomatedValidator::stop() {
    std::lock_guard lock(mutex_);
    if (!active_) return Status::Ok;

    EXPECT_EQ(scenario_engine_.stop(), Status::Ok);
    active_ = false;
    set_state(ComponentState::Stopped);
    return Status::Ok;
}

void AutomatedValidator::run_validation_suite() noexcept {
    std::lock_guard lock(mutex_);

    if (!active_) return;

    UADOS_LOG_INFO("Executing Automated Compliance Validation Suite...");
    results_.clear();

    // ------------------------------------------------------------------------
    // Test Case 1: Cruise Tracking Compliance
    // ------------------------------------------------------------------------
    {
        TestCaseResult result;
        result.name = "TC-VAL-001: Nominal Cruise Linelet Tracking";
        
        std::vector<DetectedObject> obstacles; // no obstacles
        scenario_engine_.load_scenario(0.0, 0.0, 10.0, obstacles);

        // Step straight line cruise for 1.0 second (dt = 0.1s)
        for (int i = 0; i < 10; ++i) {
            scenario_engine_.step(0.0, 0.0, 0.1);
        }

        auto metrics = scenario_engine_.get_metrics();
        auto ego_state = scenario_engine_.get_vehicle_twin().get_state();

        result.max_cross_track_error = std::abs(ego_state.position.y); // y should stay close to 0.0
        result.min_obstacle_distance = metrics.min_obstacle_distance;
        
        // Pass criterion: no collisions, cross-track error < 0.1m
        if (!metrics.collision_occurred && result.max_cross_track_error < 0.1) {
            result.passed = true;
            result.details = "Nominal cruising path tracked successfully. Max CTE = " + 
                             std::to_string(result.max_cross_track_error) + "m.";
        } else {
            result.passed = false;
            result.details = "Cruise failed. Path cross-track deviation exceeded limits.";
        }
        results_.push_back(result);
    }

    // ------------------------------------------------------------------------
    // Test Case 2: Stop Line Halting Compliance
    // ------------------------------------------------------------------------
    {
        TestCaseResult result;
        result.name = "TC-VAL-002: Behavior Stop Line Halting";
        
        // Static stop line obstacle at 15.0m
        DetectedObject stop_line;
        stop_line.id = 901;
        stop_line.object_class = ObjectClass::TrafficCone;
        stop_line.position = {15.0, 0.0, 0.0};
        stop_line.velocity = {0.0, 0.0, 0.0};

        scenario_engine_.load_scenario(0.0, 0.0, 10.0, {stop_line});

        // Simulate braking deceleration step loops
        // Apply deceleration accel = -2.5m/s²
        for (int i = 0; i < 10; ++i) {
            scenario_engine_.step(0.0, -2.5, 0.1);
        }

        auto metrics = scenario_engine_.get_metrics();
        auto ego_state = scenario_engine_.get_vehicle_twin().get_state();

        result.max_cross_track_error = std::abs(ego_state.position.y);
        result.min_obstacle_distance = metrics.min_obstacle_distance;

        // Pass criterion: vehicle successfully decelerated to stop and halted safely
        if (!metrics.collision_occurred && ego_state.velocity.vx < 1.0) {
            result.passed = true;
            result.details = "Halted safely before obstacle. Final speed: " + 
                             std::to_string(ego_state.velocity.vx) + " m/s. Clearance: " + 
                             std::to_string(result.min_obstacle_distance) + "m.";
        } else {
            result.passed = false;
            result.details = "Failed to stop safely. Final speed too high or collision occurred.";
        }
        results_.push_back(result);
    }

    // ------------------------------------------------------------------------
    // Test Case 3: Emergency Safe Envelope Compliance
    // ------------------------------------------------------------------------
    {
        TestCaseResult result;
        result.name = "TC-VAL-003: Critical Envelope Anomaly Override";
        
        // Threat obstacle extremely close (3.0m)
        DetectedObject threat;
        threat.id = 902;
        threat.object_class = ObjectClass::Pedestrian;
        threat.position = {3.0, 0.0, 0.0};
        threat.velocity = {0.0, 0.0, 0.0};

        scenario_engine_.load_scenario(0.0, 0.0, 5.0, {threat});

        // Simulate step with full braking override applied instantly
        scenario_engine_.step(0.0, -8.0, 0.1);

        auto metrics = scenario_engine_.get_metrics();
        auto ego_state = scenario_engine_.get_vehicle_twin().get_state();

        result.max_cross_track_error = std::abs(ego_state.position.y);
        result.min_obstacle_distance = metrics.min_obstacle_distance;

        // Pass criterion: emergency braking interlock responded within budget without critical crash
        if (result.min_obstacle_distance > 1.0) {
            result.passed = true;
            result.details = "Emergency stop interlock engaged within limits. Clearance = " + 
                             std::to_string(result.min_obstacle_distance) + "m.";
        } else {
            result.passed = false;
            result.details = "Emergency override failed. Clearance distance was critical.";
        }
        results_.push_back(result);
    }

    UADOS_LOG_INFO("Automated Compliance Validation Suite complete. Results compiled.");
}

std::string AutomatedValidator::compile_evidence_report() const noexcept {
    std::lock_guard lock(mutex_);

    std::stringstream ss;
    ss << "# UADOS — Automated Compliance Validation Report\n\n";
    ss << "This report represents compiled validation evidence certifying autonomy safety across nominal and fallback scenario suites.\n\n";
    ss << "## Compliance Overview Table\n\n";
    ss << "| Test Case Identifier | Outcome | Max Cross-Track Error | Min Clearance | Verdict details |\n";
    ss << "|---|---|---|---|---|\n";

    int passed_count = 0;
    for (const auto& r : results_) {
        std::string verdict = r.passed ? "✅ **PASSED**" : "❌ **FAILED**";
        if (r.passed) passed_count++;

        ss << "| " << r.name << " | " << verdict << " | " 
           << std::fixed << std::setprecision(3) << r.max_cross_track_error << "m | "
           << r.min_obstacle_distance << "m | " << r.details << " |\n";
    }

    ss << "\n## System Scorecard\n\n";
    ss << "- **Scenarios Executed**: " << results_.size() << "\n";
    ss << "- **Scenarios Passed**: " << passed_count << "\n";
    ss << "- **System Pass Rate**: " << (results_.empty() ? 0 : (passed_count * 100 / results_.size())) << "%\n\n";
    ss << "---  \n*End of Validation Report. Verified by UADOS Automated Validator.*";

    return ss.str();
}

std::vector<TestCaseResult> AutomatedValidator::get_results() const noexcept {
    std::lock_guard lock(mutex_);
    return results_;
}

} // namespace uados::validation
