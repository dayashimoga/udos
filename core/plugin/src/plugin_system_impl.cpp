#include "uados/plugin/plugin.hpp"
#include "uados/logging.hpp"

#include <mutex>
#include <unordered_map>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace uados::core {

UADOS_DECLARE_LOGGER("core.plugin")

/// @brief Plugin System implementation with dynamic loading.
///
/// Loads plugins from shared libraries (.so on Linux, .dll on Windows).
/// Each plugin library must export:
///   - uados_plugin_create() → IPlugin*
///   - uados_plugin_destroy(IPlugin*) → void
///   - uados_plugin_name() → const char*
class PluginSystemImpl final : public IPluginSystem {
public:
    PluginSystemImpl() = default;

    ~PluginSystemImpl() override {
        // Unload all plugins in reverse order
        std::vector<PluginId> ids;
        for (const auto& [id, _] : plugins_) {
            ids.push_back(id);
        }
        std::reverse(ids.begin(), ids.end());
        for (const auto& id : ids) {
            unload(id);
        }
    }

    [[nodiscard]] size_t scan(std::string_view path) override {
        namespace fs = std::filesystem;

        size_t count = 0;
        std::string dir(path);

        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            UADOS_LOG_WARN("Plugin directory not found: {}", dir);
            return 0;
        }

        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;

            auto ext = entry.path().extension().string();
#ifdef _WIN32
            if (ext != ".dll") continue;
#else
            if (ext != ".so") continue;
#endif

            auto lib_path = entry.path().string();
            UADOS_LOG_DEBUG("Discovered plugin library: {}", lib_path);
            discovered_.push_back(lib_path);
            count++;
        }

        UADOS_LOG_INFO("Scanned '{}': found {} plugin libraries", dir, count);
        return count;
    }

    [[nodiscard]] Result<PluginId> load(std::string_view library_path) override {
        std::lock_guard lock(mutex_);

        std::string lib_path(library_path);

        // Load the shared library
        void* handle = nullptr;
#ifdef _WIN32
        handle = LoadLibraryA(lib_path.c_str());
        if (!handle) {
            auto err = "LoadLibrary failed: " + std::to_string(GetLastError());
            UADOS_LOG_ERROR("Failed to load plugin '{}': {}", lib_path, err);
            return Result<PluginId>::error(Status::Error, err);
        }
#else
        handle = dlopen(lib_path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle) {
            auto err = std::string(dlerror());
            UADOS_LOG_ERROR("Failed to load plugin '{}': {}", lib_path, err);
            return Result<PluginId>::error(Status::Error, err);
        }
#endif

        // Resolve entry points
        PluginFactory create_fn = nullptr;
        PluginDestructor destroy_fn = nullptr;

#ifdef _WIN32
        create_fn = reinterpret_cast<PluginFactory>(
            GetProcAddress(static_cast<HMODULE>(handle), "uados_plugin_create"));
        destroy_fn = reinterpret_cast<PluginDestructor>(
            GetProcAddress(static_cast<HMODULE>(handle), "uados_plugin_destroy"));
#else
        create_fn = reinterpret_cast<PluginFactory>(
            dlsym(handle, "uados_plugin_create"));
        destroy_fn = reinterpret_cast<PluginDestructor>(
            dlsym(handle, "uados_plugin_destroy"));
#endif

        if (!create_fn || !destroy_fn) {
            UADOS_LOG_ERROR("Plugin '{}' missing required entry points", lib_path);
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
            return Result<PluginId>::error(Status::Error,
                                           "Missing uados_plugin_create/destroy");
        }

        // Create the plugin instance
        IPlugin* plugin = create_fn();
        if (!plugin) {
            UADOS_LOG_ERROR("Plugin '{}' create_fn returned null", lib_path);
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
            return Result<PluginId>::error(Status::Error, "create_fn returned null");
        }

        PluginId id(plugin->name());

        // Check for duplicates
        if (plugins_.contains(id)) {
            destroy_fn(plugin);
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
            return Result<PluginId>::error(Status::AlreadyExists,
                                           "Plugin '" + id + "' already loaded");
        }

        // Store the loaded plugin
        auto& entry = plugins_[id];
        entry.plugin = plugin;
        entry.handle = handle;
        entry.destroy_fn = destroy_fn;
        entry.info = PluginInfo{
            .id = id,
            .display_name = std::string(plugin->name()),
            .description = std::string(plugin->description()),
            .version = plugin->version(),
            .library_path = lib_path,
            .dependencies = plugin->dependencies(),
            .capabilities = plugin->capabilities(),
            .loaded = true,
            .running = false,
        };

        UADOS_LOG_INFO("Loaded plugin '{}' v{}.{}.{} from '{}'",
                       id, plugin->version().major,
                       plugin->version().minor,
                       plugin->version().patch, lib_path);

        return Result<PluginId>::success(id);
    }

    [[nodiscard]] Status unload(const PluginId& id) override {
        std::lock_guard lock(mutex_);

        auto it = plugins_.find(id);
        if (it == plugins_.end()) return Status::NotFound;

        auto& entry = it->second;

        // Stop if running
        if (entry.info.running) {
            entry.plugin->stop();
            entry.info.running = false;
        }

        // Destroy the plugin instance
        entry.destroy_fn(entry.plugin);
        entry.plugin = nullptr;

        // Close the shared library
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(entry.handle));
#else
        dlclose(entry.handle);
#endif

        UADOS_LOG_INFO("Unloaded plugin '{}'", id);
        plugins_.erase(it);

        return Status::Ok;
    }

    [[nodiscard]] PluginInfo info(const PluginId& id) const override {
        std::lock_guard lock(mutex_);
        auto it = plugins_.find(id);
        if (it == plugins_.end()) return {};
        return it->second.info;
    }

    [[nodiscard]] std::vector<PluginInfo> all_plugins() const override {
        std::lock_guard lock(mutex_);
        std::vector<PluginInfo> result;
        result.reserve(plugins_.size());
        for (const auto& [_, entry] : plugins_) {
            result.push_back(entry.info);
        }
        return result;
    }

    [[nodiscard]] std::vector<PluginId> query_capability(
        std::string_view capability) const override
    {
        std::lock_guard lock(mutex_);
        std::vector<PluginId> result;
        std::string cap(capability);

        for (const auto& [id, entry] : plugins_) {
            for (const auto& c : entry.info.capabilities) {
                if (c == cap) {
                    result.push_back(id);
                    break;
                }
            }
        }
        return result;
    }

    [[nodiscard]] IPlugin* get(const PluginId& id) override {
        std::lock_guard lock(mutex_);
        auto it = plugins_.find(id);
        if (it == plugins_.end()) return nullptr;
        return it->second.plugin;
    }

private:
    struct LoadedPlugin {
        IPlugin* plugin{nullptr};
        void* handle{nullptr};
        PluginDestructor destroy_fn{nullptr};
        PluginInfo info;
    };

    mutable std::mutex mutex_;
    std::unordered_map<PluginId, LoadedPlugin> plugins_;
    std::vector<std::string> discovered_;
};

/// Factory function
std::unique_ptr<IPluginSystem> create_plugin_system() {
    return std::make_unique<PluginSystemImpl>();
}

} // namespace uados::core
