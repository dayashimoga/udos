#pragma once

/// @file kernel.hpp
/// @brief Universal Autonomous Driving OS Kernel Interface.

#include "uados/types.hpp"
#include "uados/event_bus/event_bus.hpp"
#include "uados/scheduler/scheduler.hpp"
#include "uados/health/health_monitor.hpp"
#include "uados/lifecycle/lifecycle_manager.hpp"
#include "uados/plugin/plugin.hpp"
#include "uados/kernel/config_manager.hpp"

#include <memory>
#include <string>

namespace uados::core {

/// @brief Universal Autonomous Driving OS Kernel Interface
///
/// Ties together all core platform subsystems: Event Bus, Scheduler,
/// Health Monitor, Lifecycle Manager, Plugin System, and Configuration Manager.
class IKernel {
public:
    virtual ~IKernel() = default;

    /// Initialize the kernel and all core systems using the provided master configuration file
    /// @param config_path Path to the master YAML config file
    /// @return Status::Ok on success
    [[nodiscard]] virtual Status init(const std::string& config_path) = 0;

    /// Run the kernel loop (blocks the calling thread until shutdown)
    virtual void run() = 0;

    /// Request clean shutdown of the kernel and all subsystems
    virtual void shutdown() = 0;

    // -- Component Accessors --

    [[nodiscard]] virtual IEventBus& event_bus() = 0;
    [[nodiscard]] virtual IScheduler& scheduler() = 0;
    [[nodiscard]] virtual IHealthMonitor& health_monitor() = 0;
    [[nodiscard]] virtual ILifecycleManager& lifecycle_manager() = 0;
    [[nodiscard]] virtual IPluginSystem& plugin_system() = 0;
    [[nodiscard]] virtual IConfigManager& config_manager() = 0;
};

/// Factory function to create a new Kernel instance
[[nodiscard]] std::unique_ptr<IKernel> create_kernel();

} // namespace uados::core
