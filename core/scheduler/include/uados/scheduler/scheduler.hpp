#pragma once

/// @file scheduler.hpp
/// @brief Deterministic task scheduler interface.
///
/// Implements Rate-Monotonic Scheduling (RMS) with deadline monitoring.
/// All periodic tasks in UADOS register with the scheduler, which
/// ensures deterministic execution ordering based on priority.

#include "uados/types.hpp"

#include <chrono>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace uados::core {

// ============================================================================
// Task Types
// ============================================================================

/// Task priority levels (higher number = higher priority)
enum class Priority : uint8_t {
    Lowest   = 0,
    Low      = 2,
    Normal   = 5,
    High     = 7,
    Critical = 9,
    Safety   = 10  ///< Reserved for safety-critical tasks
};

/// Task execution callback
using TaskFunction = std::function<void()>;

/// Task configuration
struct TaskConfig {
    std::string name;            ///< Human-readable task name
    TaskFunction function;       ///< Function to execute
    Priority priority{Priority::Normal};
    Duration period{};           ///< Execution period (0 = one-shot)
    Duration deadline{};         ///< Maximum allowed execution time
    Duration offset{};           ///< Initial offset from scheduler start
    bool enabled{true};          ///< Whether the task is active
};

/// Runtime task statistics
struct TaskStats {
    TaskId id{0};
    std::string name;
    Priority priority{Priority::Normal};
    Duration period{};
    uint64_t total_executions{0};
    uint64_t deadline_misses{0};
    Duration last_execution_time{};
    Duration avg_execution_time{};
    Duration max_execution_time{};
    Duration min_execution_time{Duration::max()};
    Timestamp last_run{};
};

// ============================================================================
// Scheduler Interface
// ============================================================================

/// @brief Deterministic task scheduler interface.
///
/// Features:
/// - Rate-Monotonic Scheduling (RMS)
/// - Deadline monitoring with violation reporting
/// - Task statistics collection
/// - Dynamic task registration/deregistration
class IScheduler {
public:
    virtual ~IScheduler() = default;

    /// Register a periodic task
    /// @param config Task configuration
    /// @return Unique task ID
    [[nodiscard]] virtual TaskId register_task(const TaskConfig& config) = 0;

    /// Unregister a task
    /// @param id Task ID to remove
    virtual void unregister_task(TaskId id) = 0;

    /// Enable or disable a task
    virtual void set_enabled(TaskId id, bool enabled) = 0;

    /// Run one scheduler cycle (execute all ready tasks)
    virtual void run_cycle() = 0;

    /// Start the scheduler loop (blocks until stop() is called)
    virtual void run() = 0;

    /// Stop the scheduler loop
    virtual void stop() = 0;

    /// Check if the scheduler is running
    [[nodiscard]] virtual bool is_running() const = 0;

    /// Get statistics for a specific task
    [[nodiscard]] virtual TaskStats task_stats(TaskId id) const = 0;

    /// Get statistics for all tasks
    [[nodiscard]] virtual std::vector<TaskStats> all_stats() const = 0;

    /// Get total deadline misses across all tasks
    [[nodiscard]] virtual uint64_t total_deadline_misses() const = 0;

    /// Get the current scheduler tick count
    [[nodiscard]] virtual uint64_t tick_count() const = 0;
};

/// Factory function to create a new Scheduler instance
[[nodiscard]] std::unique_ptr<IScheduler> create_scheduler(
    Duration tick_interval = std::chrono::milliseconds(1));

} // namespace uados::core

