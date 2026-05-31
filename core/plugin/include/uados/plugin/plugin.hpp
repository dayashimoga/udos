#pragma once

/// @file plugin.hpp
/// @brief Plugin system interface for dynamically-loaded extensions.
///
/// All major UADOS subsystems (drivers, sensors, perception algorithms,
/// controllers) are implemented as plugins. This interface manages
/// plugin discovery, loading, versioning, and lifecycle.

#include "uados/types.hpp"
#include "uados/component.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uados::core {

// ============================================================================
// Plugin Types
// ============================================================================

/// Plugin dependency declaration
struct PluginDependency {
    std::string name;       ///< Required plugin name
    Version min_version;    ///< Minimum required version
    bool optional{false};   ///< If true, plugin works without this dep
};

/// Plugin metadata (from manifest or query)
struct PluginInfo {
    PluginId id;
    std::string display_name;
    std::string description;
    Version version;
    std::string author;
    std::string library_path;                    ///< Path to .so/.dll
    std::vector<PluginDependency> dependencies;
    std::vector<std::string> capabilities;       ///< Capability tags
    bool loaded{false};
    bool running{false};
};

// ============================================================================
// Plugin Interface (implemented by plugin authors)
// ============================================================================

/// Forward declaration of plugin context
class PluginContext;

/// @brief Interface that every plugin must implement.
///
/// Plugins are dynamically-loaded shared libraries that implement
/// this interface. The plugin system loads them via dlopen/LoadLibrary
/// and manages their lifecycle.
class IPlugin {
public:
    virtual ~IPlugin() = default;

    /// Plugin name
    [[nodiscard]] virtual std::string_view name() const = 0;

    /// Plugin version
    [[nodiscard]] virtual Version version() const = 0;

    /// Plugin description
    [[nodiscard]] virtual std::string_view description() const = 0;

    /// Declared dependencies
    [[nodiscard]] virtual std::vector<PluginDependency> dependencies() const = 0;

    /// Capability tags this plugin provides
    [[nodiscard]] virtual std::vector<std::string> capabilities() const = 0;

    /// Initialize the plugin with context (event bus, config, etc.)
    [[nodiscard]] virtual Status init(PluginContext& ctx) = 0;

    /// Start the plugin (begin processing)
    [[nodiscard]] virtual Status start() = 0;

    /// Stop the plugin (cease processing)
    [[nodiscard]] virtual Status stop() = 0;
};

/// Plugin factory function type (exported by shared library)
using PluginFactory = IPlugin* (*)();
using PluginDestructor = void (*)(IPlugin*);

/// Plugin context provided to plugins during init
class PluginContext {
public:
    virtual ~PluginContext() = default;

    /// Get the event bus
    [[nodiscard]] virtual IEventBus& event_bus() = 0;

    /// Get plugin configuration
    [[nodiscard]] virtual const Config& config() const = 0;

    /// Get a reference to another plugin by name
    [[nodiscard]] virtual IPlugin* get_plugin(std::string_view name) = 0;

    /// Get the component ID assigned to this plugin
    [[nodiscard]] virtual const ComponentId& component_id() const = 0;
};

// ============================================================================
// Plugin System Interface
// ============================================================================

/// @brief Manages discovery, loading, and lifecycle of plugins.
class IPluginSystem {
public:
    virtual ~IPluginSystem() = default;

    /// Scan a directory for plugin libraries
    /// @param path Directory to scan for .so/.dll files
    /// @return Number of plugins discovered
    [[nodiscard]] virtual size_t scan(std::string_view path) = 0;

    /// Load a plugin from a shared library
    /// @param library_path Path to the .so/.dll file
    /// @return Plugin ID on success
    [[nodiscard]] virtual Result<PluginId> load(std::string_view library_path) = 0;

    /// Unload a plugin
    /// @param id Plugin to unload (must be stopped first)
    [[nodiscard]] virtual Status unload(const PluginId& id) = 0;

    /// Get information about a loaded plugin
    [[nodiscard]] virtual PluginInfo info(const PluginId& id) const = 0;

    /// Get all loaded plugins
    [[nodiscard]] virtual std::vector<PluginInfo> all_plugins() const = 0;

    /// Query plugins by capability
    /// @param capability Capability tag to search for
    /// @return List of plugin IDs that provide this capability
    [[nodiscard]] virtual std::vector<PluginId> query_capability(
        std::string_view capability) const = 0;

    /// Get a plugin instance by ID
    [[nodiscard]] virtual IPlugin* get(const PluginId& id) = 0;
};

/// Factory function to create a new Plugin System instance
[[nodiscard]] std::unique_ptr<IPluginSystem> create_plugin_system();

// ============================================================================
// Plugin Registration Macros
// ============================================================================

/// Macro for plugin shared library entry points
/// Usage: UADOS_REGISTER_PLUGIN(MyPluginClass)
#define UADOS_REGISTER_PLUGIN(PluginClass)                           \
    extern "C" {                                                      \
        ::uados::core::IPlugin* uados_plugin_create() {              \
            return new PluginClass();                                  \
        }                                                             \
        void uados_plugin_destroy(::uados::core::IPlugin* plugin) {  \
            delete plugin;                                             \
        }                                                             \
        const char* uados_plugin_name() {                             \
            return #PluginClass;                                       \
        }                                                             \
    }

} // namespace uados::core

