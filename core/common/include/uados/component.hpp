#pragma once

/// @file component.hpp
/// @brief Base component interface for all UADOS subsystems.
///
/// Every major UADOS component implements IComponent, providing
/// a uniform lifecycle (init → start → stop), health reporting,
/// and configuration management.

#include "uados/types.hpp"
#include "uados/version.hpp"

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

#include <string>
#include <string_view>

namespace uados::core {

/// Configuration container (wraps YAML node)
using Config = YAML::Node;

/// Abstract base class for all UADOS components
class IComponent {
public:
    virtual ~IComponent() = default;

    // -- Lifecycle --

    /// Initialize the component with the given configuration
    /// @param config YAML configuration node
    /// @return Status::Ok on success
    [[nodiscard]] virtual Status init(const Config& config) = 0;

    /// Start the component (begin processing)
    /// @return Status::Ok on success
    [[nodiscard]] virtual Status start() = 0;

    /// Stop the component (cease processing, release resources)
    /// @return Status::Ok on success
    [[nodiscard]] virtual Status stop() = 0;

    // -- Identity --

    /// Component name (e.g., "perception.detection")
    [[nodiscard]] virtual std::string_view name() const = 0;

    /// Component version
    [[nodiscard]] virtual Version version() const = 0;

    // -- Health --

    /// Current health status
    [[nodiscard]] virtual HealthStatus health() const = 0;

    /// Current lifecycle state
    [[nodiscard]] virtual ComponentState state() const = 0;

    // -- Configuration --

    /// Update configuration at runtime
    /// @param config New YAML configuration
    virtual void reconfigure(const Config& config) = 0;
};

/// Base implementation providing common lifecycle state management
class ComponentBase : public IComponent {
public:
    [[nodiscard]] HealthStatus health() const override { return health_; }
    [[nodiscard]] ComponentState state() const override { return state_; }

    void reconfigure(const Config& /*config*/) override {
        // Default: no-op. Override in derived classes for hot-reconfiguration.
    }

protected:
    void set_health(HealthStatus h) noexcept { health_ = h; }
    void set_state(ComponentState s) noexcept { state_ = s; }

private:
    HealthStatus health_{HealthStatus::Unknown};
    ComponentState state_{ComponentState::Loaded};
};

} // namespace uados::core
