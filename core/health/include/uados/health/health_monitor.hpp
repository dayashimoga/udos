#pragma once

/// @file health_monitor.hpp
/// @brief Health monitoring and watchdog interface.
///
/// The Health Monitor tracks the health of all registered components
/// using configurable watchdog timeouts. Components must send periodic
/// heartbeats; missed heartbeats trigger health degradation alerts.

#include "uados/types.hpp"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace uados::core {

// ============================================================================
// Health Types
// ============================================================================

/// Health report for a single component
struct ComponentHealth {
    ComponentId id;
    HealthStatus status{HealthStatus::Unknown};
    ComponentState state{ComponentState::Loaded};
    Duration watchdog_timeout{};
    Timestamp last_heartbeat{};
    uint64_t missed_heartbeats{0};
    uint64_t total_heartbeats{0};
    std::string last_error;
};

/// System-wide health summary
struct SystemHealth {
    HealthStatus overall_status{HealthStatus::Unknown};
    size_t total_components{0};
    size_t healthy_count{0};
    size_t degraded_count{0};
    size_t unhealthy_count{0};
    Timestamp timestamp{};
};

/// Health change callback
using HealthCallback = std::function<void(
    const ComponentId& id,
    HealthStatus old_status,
    HealthStatus new_status
)>;

// ============================================================================
// Health Monitor Interface
// ============================================================================

/// @brief Monitors component health via watchdog heartbeats.
///
/// Features:
/// - Per-component watchdog timeouts
/// - Automatic health degradation on missed heartbeats
/// - Health change callbacks for alerting
/// - System-wide health aggregation
class IHealthMonitor {
public:
    virtual ~IHealthMonitor() = default;

    /// Register a component for health monitoring
    /// @param id Unique component identifier
    /// @param timeout Watchdog timeout (max time between heartbeats)
    virtual void register_component(
        const ComponentId& id,
        Duration timeout) = 0;

    /// Unregister a component
    virtual void unregister_component(const ComponentId& id) = 0;

    /// Send a heartbeat from a component
    /// @param id Component identifier
    virtual void heartbeat(const ComponentId& id) = 0;

    /// Report an error for a component
    /// @param id Component identifier
    /// @param error Error description
    virtual void report_error(const ComponentId& id, const std::string& error) = 0;

    /// Get health status of a specific component
    [[nodiscard]] virtual ComponentHealth get_health(const ComponentId& id) const = 0;

    /// Get health of all registered components
    [[nodiscard]] virtual std::vector<ComponentHealth> all_health() const = 0;

    /// Get system-wide health summary
    [[nodiscard]] virtual SystemHealth system_health() const = 0;

    /// Register a callback for health status changes
    /// @param callback Function called when any component's health changes
    virtual void on_health_change(HealthCallback callback) = 0;

    /// Check all watchdogs (called periodically by scheduler)
    virtual void check_watchdogs() = 0;
};

/// Factory function to create a new Health Monitor instance
[[nodiscard]] std::unique_ptr<IHealthMonitor> create_health_monitor();

} // namespace uados::core

