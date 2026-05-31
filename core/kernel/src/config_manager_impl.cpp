#include "uados/kernel/config_manager.hpp"
#include "uados/logging.hpp"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace uados::core {

UADOS_DECLARE_LOGGER("core.config")

class ConfigManagerImpl final : public IConfigManager {
public:
    ConfigManagerImpl() = default;
    ~ConfigManagerImpl() override = default;

    Status load_file(const std::string& path) override {
        std::lock_guard lock(mutex_);

        if (!std::filesystem::exists(path)) {
            UADOS_LOG_ERROR("Config file does not exist: {}", path);
            return Status::NotFound;
        }

        try {
            root_ = YAML::LoadFile(path);
            UADOS_LOG_INFO("Successfully loaded configuration file: {}", path);
            return Status::Ok;
        } catch (const std::exception& e) {
            UADOS_LOG_ERROR("Failed to parse config file '{}': {}", path, e.what());
            return Status::Error;
        }
    }

    Config get_component_config(std::string_view component_name) const override {
        std::lock_guard lock(mutex_);

        if (!root_ || !root_["components"]) {
            return Config{}; // Return empty node
        }

        auto component_node = root_["components"][std::string(component_name)];
        if (!component_node) {
            UADOS_LOG_DEBUG("No configuration found for component '{}'", component_name);
            return Config{};
        }

        return component_node;
    }

    Config get_system_config(std::string_view system_name) const override {
        std::lock_guard lock(mutex_);

        if (!root_) {
            return Config{};
        }

        auto system_node = root_[std::string(system_name)];
        if (!system_node) {
            UADOS_LOG_DEBUG("No configuration found for core system '{}'", system_name);
            return Config{};
        }

        return system_node;
    }

    const Config& get_root() const override {
        std::lock_guard lock(mutex_);
        return root_;
    }

private:
    mutable std::mutex mutex_;
    Config root_;
};

std::unique_ptr<IConfigManager> create_config_manager() {
    return std::make_unique<ConfigManagerImpl>();
}

} // namespace uados::core
