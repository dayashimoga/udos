#pragma once

/// @file config_manager.hpp
/// @brief Configuration manager for loading and querying YAML settings.

#include "uados/types.hpp"
#include "uados/component.hpp"
#include <yaml-cpp/yaml.h>
#include <string>
#include <string_view>

namespace uados::core {

/// @brief Interface for managing UADOS system-wide and component-specific configuration.
class IConfigManager {
public:
    virtual ~IConfigManager() = default;

    /// Load configuration from a YAML file
    /// @param path Path to the YAML file
    /// @return Status::Ok on success
    [[nodiscard]] virtual Status load_file(const std::string& path) = 0;

    /// Get configuration for a specific component
    /// @param component_name Name of the component (e.g., "perception.detection")
    /// @return Config (YAML::Node) for the component (empty node if not found)
    [[nodiscard]] virtual Config get_component_config(std::string_view component_name) const = 0;

    /// Get configuration for a core system
    /// @param system_name Name of the core system (e.g., "scheduler", "health")
    /// @return Config (YAML::Node) for the system (empty node if not found)
    [[nodiscard]] virtual Config get_system_config(std::string_view system_name) const = 0;

    /// Get the raw root YAML node
    [[nodiscard]] virtual const Config& get_root() const = 0;
};

/// Factory function to create a new ConfigManager instance
std::unique_ptr<IConfigManager> create_config_manager();

} // namespace uados::core
