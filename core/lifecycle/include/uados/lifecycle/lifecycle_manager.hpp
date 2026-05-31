#pragma once

/// @file lifecycle_manager.hpp
/// @brief Component lifecycle state machine interface.
///
/// Manages state transitions for all UADOS components:
/// Loaded → Initialized → Running → Paused → Stopping → Stopped
/// With error recovery paths.

#include "uados/types.hpp"
#include "uados/component.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uados::core {

/// Lifecycle transition event
struct LifecycleEvent {
    ComponentId component_id;
    ComponentState old_state;
    ComponentState new_state;
    Timestamp timestamp{};
    std::string reason;
};

/// Lifecycle transition callback
using LifecycleCallback = std::function<void(const LifecycleEvent&)>;

/// @brief Manages component lifecycle state transitions.
///
/// Enforces valid state transitions and notifies observers.
/// Integrates with HealthMonitor for health-aware lifecycle decisions.
class ILifecycleManager {
public:
    virtual ~ILifecycleManager() = default;

    /// Register a component for lifecycle management
    virtual void register_component(
        const ComponentId& id,
        std::shared_ptr<IComponent> component) = 0;

    /// Unregister a component
    virtual void unregister_component(const ComponentId& id) = 0;

    /// Initialize a component
    [[nodiscard]] virtual Status initialize(
        const ComponentId& id, const Config& config) = 0;

    /// Start a component
    [[nodiscard]] virtual Status start(const ComponentId& id) = 0;

    /// Pause a component
    [[nodiscard]] virtual Status pause(const ComponentId& id) = 0;

    /// Resume a paused component
    [[nodiscard]] virtual Status resume(const ComponentId& id) = 0;

    /// Stop a component
    [[nodiscard]] virtual Status stop(const ComponentId& id) = 0;

    /// Initialize all registered components
    [[nodiscard]] virtual Status initialize_all(const Config& config) = 0;

    /// Start all initialized components
    [[nodiscard]] virtual Status start_all() = 0;

    /// Stop all running components
    [[nodiscard]] virtual Status stop_all() = 0;

    /// Get the state of a component
    [[nodiscard]] virtual ComponentState get_state(const ComponentId& id) const = 0;

    /// Get all component states
    [[nodiscard]] virtual std::vector<std::pair<ComponentId, ComponentState>>
        all_states() const = 0;

    /// Register a lifecycle event callback
    virtual void on_lifecycle_event(LifecycleCallback callback) = 0;
};

/// Factory function to create a new Lifecycle Manager instance
[[nodiscard]] std::unique_ptr<ILifecycleManager> create_lifecycle_manager();

} // namespace uados::core

