#include "uados/resource_profiler.hpp"

#include <cmath>
#include <algorithm>
#include <numeric>

namespace uados::common {

void ResourceProfiler::start_timer() noexcept {
    std::lock_guard lock(mutex_);
    start_time_ = Clock::now();
}

double ResourceProfiler::stop_timer() noexcept {
    std::lock_guard lock(mutex_);
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_);
    double duration_s = static_cast<double>(elapsed.count()) / 1e6;

    latencies_.push_back(duration_s);
    return duration_s;
}

size_t ResourceProfiler::audit_leak_stats(bool& out_leaks_detected) noexcept {
    std::lock_guard lock(mutex_);
    out_leaks_detected = false;
    leak_audit_cycles_++;

    // Simulating a memory footprint leak under prolonged test cycles
    // If cycles exceed 100, we simulate a small leak, otherwise stable 16MB
    if (leak_audit_cycles_ > 100) {
        mock_memory_usage_ += 1024 * 4; // leak 4KB per check
        out_leaks_detected = true;
    }

    return mock_memory_usage_;
}

double ResourceProfiler::get_jitter(
    double expected_period_s,
    const std::vector<double>& actual_durations) const noexcept {
    
    if (actual_durations.empty()) {
        return 0.0;
    }

    double total_deviation = 0.0;
    for (double d : actual_durations) {
        total_deviation += std::abs(d - expected_period_s);
    }

    return total_deviation / static_cast<double>(actual_durations.size());
}

void ResourceProfiler::reset() noexcept {
    std::lock_guard lock(mutex_);
    latencies_.clear();
    leak_audit_cycles_ = 0;
    mock_memory_usage_ = 1024 * 1024 * 16;
}

} // namespace uados::common
