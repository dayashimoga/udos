#include "uados/scheduler/scheduler.hpp"
#include "uados/logging.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace uados::core {

UADOS_DECLARE_LOGGER("core.scheduler")

/// @brief Rate-Monotonic Scheduler implementation.
///
/// Executes registered tasks in priority order at their configured periods.
/// Tracks execution time, deadline misses, and jitter statistics.
class SchedulerImpl final : public IScheduler {
public:
    explicit SchedulerImpl(Duration tick_interval = std::chrono::milliseconds(1))
        : tick_interval_(tick_interval) {}

    ~SchedulerImpl() override {
        if (running_) {
            stop();
        }
    }

    [[nodiscard]] TaskId register_task(const TaskConfig& config) override {
        std::lock_guard lock(mutex_);

        auto id = next_id_++;
        auto& task = tasks_[id];
        task.config = config;
        task.stats.id = id;
        task.stats.name = config.name;
        task.stats.priority = config.priority;
        task.stats.period = config.period;
        task.next_run = Clock::now() + config.offset;

        UADOS_LOG_INFO("Registered task '{}' id={} priority={} period={}ms",
                       config.name, id, static_cast<int>(config.priority),
                       std::chrono::duration_cast<std::chrono::milliseconds>(config.period).count());

        return id;
    }

    void unregister_task(TaskId id) override {
        std::lock_guard lock(mutex_);
        tasks_.erase(id);
    }

    void set_enabled(TaskId id, bool enabled) override {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(id);
        if (it != tasks_.end()) {
            it->second.config.enabled = enabled;
        }
    }

    void run_cycle() override {
        auto now = Clock::now();
        tick_count_++;

        // Collect ready tasks
        std::vector<TaskId> ready;
        {
            std::lock_guard lock(mutex_);
            for (auto& [id, task] : tasks_) {
                if (!task.config.enabled) continue;
                if (task.config.period == Duration::zero()) continue; // skip one-shot
                if (now >= task.next_run) {
                    ready.push_back(id);
                }
            }
        }

        // Sort by priority (highest first — Rate-Monotonic)
        std::sort(ready.begin(), ready.end(), [this](TaskId a, TaskId b) {
            return tasks_[a].config.priority > tasks_[b].config.priority;
        });

        // Execute tasks in priority order
        for (auto id : ready) {
            execute_task(id, now);
        }
    }

    void run() override {
        running_ = true;
        UADOS_LOG_INFO("Scheduler started with tick={}us",
                       std::chrono::duration_cast<std::chrono::microseconds>(tick_interval_).count());

        while (running_) {
            auto cycle_start = Clock::now();
            run_cycle();

            auto cycle_end = Clock::now();
            auto elapsed = cycle_end - cycle_start;

            if (elapsed < tick_interval_) {
                std::this_thread::sleep_for(tick_interval_ - elapsed);
            }
        }

        UADOS_LOG_INFO("Scheduler stopped after {} ticks", tick_count_.load());
    }

    void stop() override {
        running_ = false;
    }

    [[nodiscard]] bool is_running() const override {
        return running_.load();
    }

    [[nodiscard]] TaskStats task_stats(TaskId id) const override {
        std::lock_guard lock(mutex_);
        auto it = tasks_.find(id);
        if (it == tasks_.end()) return {};
        return it->second.stats;
    }

    [[nodiscard]] std::vector<TaskStats> all_stats() const override {
        std::lock_guard lock(mutex_);
        std::vector<TaskStats> result;
        result.reserve(tasks_.size());
        for (const auto& [_, task] : tasks_) {
            result.push_back(task.stats);
        }
        return result;
    }

    [[nodiscard]] uint64_t total_deadline_misses() const override {
        std::lock_guard lock(mutex_);
        uint64_t total = 0;
        for (const auto& [_, task] : tasks_) {
            total += task.stats.deadline_misses;
        }
        return total;
    }

    [[nodiscard]] uint64_t tick_count() const override {
        return tick_count_.load();
    }

private:
    struct TaskEntry {
        TaskConfig config;
        TaskStats stats;
        Timestamp next_run{};
    };

    void execute_task(TaskId id, Timestamp now) {
        TaskConfig config;
        {
            std::lock_guard lock(mutex_);
            auto it = tasks_.find(id);
            if (it == tasks_.end()) return;
            config = it->second.config;
        }

        // Execute the task function
        auto exec_start = Clock::now();

        try {
            config.function();
        } catch (const std::exception& e) {
            UADOS_LOG_ERROR("Task '{}' threw exception: {}", config.name, e.what());
        }

        auto exec_end = Clock::now();
        auto exec_time = exec_end - exec_start;

        // Update statistics
        {
            std::lock_guard lock(mutex_);
            auto it = tasks_.find(id);
            if (it == tasks_.end()) return;

            auto& task = it->second;
            auto& stats = task.stats;

            stats.total_executions++;
            stats.last_execution_time = exec_time;
            stats.last_run = exec_start;

            // Running average
            if (stats.total_executions == 1) {
                stats.avg_execution_time = exec_time;
            } else {
                auto avg_ns = stats.avg_execution_time.count();
                auto new_ns = exec_time.count();
                stats.avg_execution_time = Duration(
                    avg_ns + (new_ns - avg_ns) / static_cast<int64_t>(stats.total_executions));
            }

            if (exec_time > stats.max_execution_time) {
                stats.max_execution_time = exec_time;
            }
            if (exec_time < stats.min_execution_time) {
                stats.min_execution_time = exec_time;
            }

            // Deadline check
            if (config.deadline != Duration::zero() && exec_time > config.deadline) {
                stats.deadline_misses++;
                UADOS_LOG_WARN("Task '{}' missed deadline: {}us > {}us",
                               config.name,
                               std::chrono::duration_cast<std::chrono::microseconds>(exec_time).count(),
                               std::chrono::duration_cast<std::chrono::microseconds>(config.deadline).count());
            }

            // Schedule next run
            task.next_run += config.period;

            // If we fell behind, skip to next valid period
            if (task.next_run < now) {
                auto periods_behind = (now - task.next_run) / config.period + 1;
                task.next_run += config.period * periods_behind;
            }
        }
    }

    Duration tick_interval_;
    mutable std::mutex mutex_;
    std::unordered_map<TaskId, TaskEntry> tasks_;
    std::atomic<TaskId> next_id_{1};
    std::atomic<uint64_t> tick_count_{0};
    std::atomic<bool> running_{false};
};

/// Factory function
std::unique_ptr<IScheduler> create_scheduler(
    Duration tick_interval = std::chrono::milliseconds(1))
{
    return std::make_unique<SchedulerImpl>(tick_interval);
}

} // namespace uados::core
