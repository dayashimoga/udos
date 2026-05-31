#include "uados/health/health_monitor.hpp"
#include "uados/logging.hpp"

#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace uados::core {

UADOS_DECLARE_LOGGER("core.health")

/// @brief Health Monitor implementation with watchdog timers.
///
/// Tracks registered components' health via periodic heartbeats.
/// Automatically degrades health status when heartbeats are missed.
/// Notifies registered callbacks on health state transitions.
class HealthMonitorImpl final : public IHealthMonitor {
public:
    HealthMonitorImpl() = default;
    ~HealthMonitorImpl() override = default;

    void register_component(
        const ComponentId& id,
        Duration timeout) override
    {
        std::lock_guard lock(mutex_);

        auto& entry = components_[id];
        entry.id = id;
        entry.watchdog_timeout = timeout;
        entry.status = HealthStatus::Healthy;
        entry.last_heartbeat = Clock::now();

        UADOS_LOG_INFO("Registered component '{}' with timeout={}ms",
                       id,
                       std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    }

    void unregister_component(const ComponentId& id) override {
        std::lock_guard lock(mutex_);
        components_.erase(id);
        UADOS_LOG_INFO("Unregistered component '{}'", id);
    }

    void heartbeat(const ComponentId& id) override {
        std::lock_guard lock(mutex_);

        auto it = components_.find(id);
        if (it == components_.end()) return;

        auto& entry = it->second;
        entry.last_heartbeat = Clock::now();
        entry.total_heartbeats++;

        // Recover from degraded/unhealthy if heartbeat resumes
        if (entry.status != HealthStatus::Healthy) {
            auto old_status = entry.status;
            entry.status = HealthStatus::Healthy;
            entry.missed_heartbeats = 0;
            entry.last_error.clear();

            notify_change(id, old_status, HealthStatus::Healthy);
            UADOS_LOG_INFO("Component '{}' recovered to Healthy", id);
        }
    }

    void report_error(const ComponentId& id, const std::string& error) override {
        std::lock_guard lock(mutex_);

        auto it = components_.find(id);
        if (it == components_.end()) return;

        auto& entry = it->second;
        auto old_status = entry.status;
        entry.status = HealthStatus::Unhealthy;
        entry.last_error = error;

        if (old_status != HealthStatus::Unhealthy) {
            notify_change(id, old_status, HealthStatus::Unhealthy);
        }

        UADOS_LOG_ERROR("Component '{}' reported error: {}", id, error);
    }

    [[nodiscard]] ComponentHealth get_health(const ComponentId& id) const override {
        std::lock_guard lock(mutex_);

        auto it = components_.find(id);
        if (it == components_.end()) {
            return ComponentHealth{.id = id, .status = HealthStatus::Unknown};
        }
        return it->second;
    }

    [[nodiscard]] std::vector<ComponentHealth> all_health() const override {
        std::lock_guard lock(mutex_);
        std::vector<ComponentHealth> result;
        result.reserve(components_.size());
        for (const auto& [_, entry] : components_) {
            result.push_back(entry);
        }
        return result;
    }

    [[nodiscard]] SystemHealth system_health() const override {
        std::lock_guard lock(mutex_);

        SystemHealth health;
        health.timestamp = Clock::now();
        health.total_components = components_.size();

        for (const auto& [_, entry] : components_) {
            switch (entry.status) {
                case HealthStatus::Healthy:
                    health.healthy_count++;
                    break;
                case HealthStatus::Degraded:
                    health.degraded_count++;
                    break;
                case HealthStatus::Unhealthy:
                    health.unhealthy_count++;
                    break;
                case HealthStatus::Unknown:
                    break;
            }
        }

        // Overall status is the worst of all components
        if (health.unhealthy_count > 0) {
            health.overall_status = HealthStatus::Unhealthy;
        } else if (health.degraded_count > 0) {
            health.overall_status = HealthStatus::Degraded;
        } else if (health.healthy_count > 0) {
            health.overall_status = HealthStatus::Healthy;
        } else {
            health.overall_status = HealthStatus::Unknown;
        }

        return health;
    }

    void on_health_change(HealthCallback callback) override {
        std::lock_guard lock(mutex_);
        callbacks_.push_back(std::move(callback));
    }

    void check_watchdogs() override {
        auto now = Clock::now();
        std::lock_guard lock(mutex_);

        for (auto& [id, entry] : components_) {
            if (entry.watchdog_timeout == Duration::zero()) continue;

            auto elapsed = now - entry.last_heartbeat;

            if (elapsed > entry.watchdog_timeout * 3) {
                // 3x timeout → Unhealthy
                if (entry.status != HealthStatus::Unhealthy) {
                    auto old_status = entry.status;
                    entry.status = HealthStatus::Unhealthy;
                    entry.missed_heartbeats++;
                    entry.last_error = "Watchdog timeout (3x) - component unresponsive";

                    notify_change(id, old_status, HealthStatus::Unhealthy);
                    UADOS_LOG_ERROR("Component '{}' UNHEALTHY: no heartbeat for {}ms",
                                   id,
                                   std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
                }
            } else if (elapsed > entry.watchdog_timeout) {
                // 1x timeout → Degraded
                if (entry.status == HealthStatus::Healthy) {
                    auto old_status = entry.status;
                    entry.status = HealthStatus::Degraded;
                    entry.missed_heartbeats++;

                    notify_change(id, old_status, HealthStatus::Degraded);
                    UADOS_LOG_WARN("Component '{}' DEGRADED: heartbeat late by {}ms",
                                   id,
                                   std::chrono::duration_cast<std::chrono::milliseconds>(
                                       elapsed - entry.watchdog_timeout).count());
                }
            }
        }
    }

private:
    void notify_change(const ComponentId& id,
                       HealthStatus old_status,
                       HealthStatus new_status) {
        for (auto& callback : callbacks_) {
            try {
                callback(id, old_status, new_status);
            } catch (const std::exception& e) {
                UADOS_LOG_ERROR("Health callback threw: {}", e.what());
            }
        }
    }

    mutable std::mutex mutex_;
    std::unordered_map<ComponentId, ComponentHealth> components_;
    std::vector<HealthCallback> callbacks_;
};

/// Factory function
std::unique_ptr<IHealthMonitor> create_health_monitor() {
    return std::make_unique<HealthMonitorImpl>();
}

} // namespace uados::core
