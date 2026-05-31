#pragma once

/// @file resource_profiler.hpp
/// @brief Timing latency profiler, memory leak auditor, and scheduler jitter monitor.

#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::common {

/// @brief Production Hardening Resource Profiler.
///
/// Provides microsecond-precision execution cycle timing measurements,
/// monitors scheduler jitter, and performs memory leak footprint audits.
class ResourceProfiler final {
public:
    ResourceProfiler() = default;
    ~ResourceProfiler() = default;

    /// Starts a high-resolution execution cycle timer
    void start_timer() noexcept;

    /// Stops the timer and caches measured latency (s)
    /// @return Measured latency duration in seconds
    double stop_timer() noexcept;

    /// Simulates checking active memory footprint allocations to detect leaks
    /// @param out_leaks_detected Output flag set to true if dynamic leaks are caught
    /// @return Active memory footprint size in bytes
    [[nodiscard]] size_t audit_leak_stats(bool& out_leaks_detected) noexcept;

    /// Computes loop timing jitter (s) relative to expected cycle duration
    /// @param expected_period_s Configured loop duration period (s)
    /// @param actual_durations List of measured cycle durations (s)
    /// @return Calculated timing jitter (s)
    [[nodiscard]] double get_jitter(
        double expected_period_s,
        const std::vector<double>& actual_durations) const noexcept;

    /// Clear cached latency logs
    void reset() noexcept;

private:
    mutable std::mutex mutex_;
    Timestamp start_time_{};
    std::vector<double> latencies_;
    
    size_t mock_memory_usage_{1024 * 1024 * 16}; // 16MB starting heap allocation
    int leak_audit_cycles_{0};
};

} // namespace uados::common
