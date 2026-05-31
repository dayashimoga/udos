#include "uados/kernel/kernel.hpp"
#include "uados/event_bus/event_bus_factory.hpp"
#include "uados/logging.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace uados::core {

UADOS_DECLARE_LOGGER("core.kernel")

class KernelImpl final : public IKernel {
public:
    KernelImpl()
        : event_bus_(create_event_bus()),
          health_monitor_(create_health_monitor()),
          lifecycle_manager_(create_lifecycle_manager()),
          plugin_system_(create_plugin_system()),
          config_manager_(create_config_manager()) {}

    ~KernelImpl() override {
        shutdown();
    }

    Status init(const std::string& config_path) override {
        std::lock_guard lock(mutex_);

        if (initialized_) {
            UADOS_LOG_WARN("Kernel already initialized");
            return Status::Ok;
        }

        UADOS_LOG_INFO("Initializing UADOS microkernel...");

        // 1. Load configurations
        auto status = config_manager_->load_file(config_path);
        if (status != Status::Ok) {
            UADOS_LOG_CRITICAL("Failed to load kernel configuration from '{}'", config_path);
            return status;
        }

        // 2. Initialize Scheduler
        auto scheduler_config = config_manager_->get_system_config("scheduler");
        Duration tick_interval = std::chrono::milliseconds(1); // default
        if (scheduler_config && scheduler_config["tick_interval_ms"]) {
            tick_interval = std::chrono::milliseconds(scheduler_config["tick_interval_ms"].as<int64_t>());
        }
        scheduler_ = create_scheduler(tick_interval);

        // 3. Register periodic watchdog checking task
        auto health_config = config_manager_->get_system_config("health");
        Duration watchdog_period = std::chrono::milliseconds(100); // default
        if (health_config && health_config["watchdog_period_ms"]) {
            watchdog_period = std::chrono::milliseconds(health_config["watchdog_period_ms"].as<int64_t>());
        }

        TaskConfig watchdog_task{
            .name = "core.health_watchdog",
            .function = [this]() { health_monitor_->check_watchdogs(); },
            .priority = Priority::Safety,
            .period = watchdog_period,
            .deadline = std::chrono::milliseconds(10), // must execute within 10ms
            .enabled = true
        };
        scheduler_->register_task(watchdog_task);

        // 4. Scan and load configured plugins
        auto plugin_config = config_manager_->get_system_config("plugin");
        if (plugin_config && plugin_config["directory"]) {
            auto plugin_dir = plugin_config["directory"].as<std::string>();
            UADOS_LOG_INFO("Scanning plugin directory: {}", plugin_dir);
            size_t found = plugin_system_->scan(plugin_dir);
            UADOS_LOG_INFO("Discovered {} plugins", found);

            if (plugin_config["autoload"]) {
                for (const auto& item : plugin_config["autoload"]) {
                    auto lib_name = item.as<std::string>();
                    UADOS_LOG_INFO("Autoloading plugin: {}", lib_name);
                    auto load_res = plugin_system_->load(lib_name);
                    if (!load_res.ok()) {
                        UADOS_LOG_ERROR("Failed to autoload plugin '{}': {}", lib_name, load_res.message);
                    }
                }
            }
        }

        initialized_ = true;
        UADOS_LOG_INFO("UADOS microkernel initialized successfully");
        return Status::Ok;
    }

    void run() override {
        {
            std::lock_guard lock(mutex_);
            if (!initialized_) {
                UADOS_LOG_CRITICAL("Cannot run kernel: not initialized");
                return;
            }
            if (running_) {
                UADOS_LOG_WARN("Kernel is already running");
                return;
            }
            running_ = true;
        }

        UADOS_LOG_INFO("Starting all registered subsystems...");
        auto status = lifecycle_manager_->start_all();
        if (status != Status::Ok) {
            UADOS_LOG_ERROR("One or more subsystems failed to start cleanly");
        }

        UADOS_LOG_INFO("UADOS Kernel entering main loop...");
        scheduler_->run(); // Blocks until scheduler->stop() is called

        UADOS_LOG_INFO("UADOS Kernel loop exited");
        running_ = false;
    }

    void shutdown() override {
        std::lock_guard lock(mutex_);

        if (!initialized_) return;

        UADOS_LOG_INFO("Shutting down UADOS microkernel...");

        // 1. Stop scheduler
        if (scheduler_ && scheduler_->is_running()) {
            scheduler_->stop();
        }

        // 2. Stop all subsystems cleanly
        if (lifecycle_manager_) {
            UADOS_LOG_INFO("Stopping all subsystems...");
            lifecycle_manager_->stop_all();
        }

        // 3. Unload all plugins
        if (plugin_system_) {
            UADOS_LOG_INFO("Unloading all plugins...");
            auto loaded = plugin_system_->all_plugins();
            for (const auto& p : loaded) {
                plugin_system_->unload(p.id);
            }
        }

        initialized_ = false;
        UADOS_LOG_INFO("UADOS microkernel shutdown complete");
    }

    // -- Component Accessors --
    IEventBus& event_bus() override { return *event_bus_; }
    IScheduler& scheduler() override { return *scheduler_; }
    IHealthMonitor& health_monitor() override { return *health_monitor_; }
    ILifecycleManager& lifecycle_manager() override { return *lifecycle_manager_; }
    IPluginSystem& plugin_system() override { return *plugin_system_; }
    IConfigManager& config_manager() override { return *config_manager_; }

private:
    mutable std::mutex mutex_;
    bool initialized_{false};
    std::atomic<bool> running_{false};

    std::unique_ptr<IEventBus> event_bus_;
    std::unique_ptr<IScheduler> scheduler_;
    std::unique_ptr<IHealthMonitor> health_monitor_;
    std::unique_ptr<ILifecycleManager> lifecycle_manager_;
    std::unique_ptr<IPluginSystem> plugin_system_;
    std::unique_ptr<IConfigManager> config_manager_;
};

std::unique_ptr<IKernel> create_kernel() {
    return std::make_unique<KernelImpl>();
}

} // namespace uados::core
