#include "uados/lifecycle/lifecycle_manager.hpp"
#include "uados/logging.hpp"

#include <mutex>
#include <unordered_map>

namespace uados::core {

UADOS_DECLARE_LOGGER("core.lifecycle")

/// @brief Lifecycle Manager implementation.
///
/// Enforces the component state machine:
///   Loaded → Initialized → Running ↔ Paused → Stopping → Stopped
///   Any state → Error (on failure)
///   Error → Initialized (on recovery)
class LifecycleManagerImpl final : public ILifecycleManager {
public:
    void register_component(
        const ComponentId& id,
        std::shared_ptr<IComponent> component) override
    {
        std::lock_guard lock(mutex_);
        entries_[id] = {
            .component = std::move(component),
            .state = ComponentState::Loaded
        };
        UADOS_LOG_INFO("Registered component '{}' for lifecycle management", id);
    }

    void unregister_component(const ComponentId& id) override {
        std::lock_guard lock(mutex_);
        entries_.erase(id);
    }

    [[nodiscard]] Status initialize(
        const ComponentId& id, const Config& config) override
    {
        std::lock_guard lock(mutex_);

        auto it = entries_.find(id);
        if (it == entries_.end()) return Status::NotFound;

        auto& entry = it->second;
        if (entry.state != ComponentState::Loaded &&
            entry.state != ComponentState::Error) {
            UADOS_LOG_ERROR("Cannot init '{}': invalid state {}", id,
                            static_cast<int>(entry.state));
            return Status::Error;
        }

        auto old_state = entry.state;
        auto result = entry.component->init(config);

        if (result == Status::Ok) {
            transition(id, entry, old_state, ComponentState::Initialized, "init success");
        } else {
            transition(id, entry, old_state, ComponentState::Error, "init failed");
        }

        return result;
    }

    [[nodiscard]] Status start(const ComponentId& id) override {
        std::lock_guard lock(mutex_);

        auto it = entries_.find(id);
        if (it == entries_.end()) return Status::NotFound;

        auto& entry = it->second;
        if (entry.state != ComponentState::Initialized &&
            entry.state != ComponentState::Paused) {
            UADOS_LOG_ERROR("Cannot start '{}': invalid state {}", id,
                            static_cast<int>(entry.state));
            return Status::Error;
        }

        auto old_state = entry.state;
        auto result = entry.component->start();

        if (result == Status::Ok) {
            transition(id, entry, old_state, ComponentState::Running, "started");
        } else {
            transition(id, entry, old_state, ComponentState::Error, "start failed");
        }

        return result;
    }

    [[nodiscard]] Status pause(const ComponentId& id) override {
        std::lock_guard lock(mutex_);

        auto it = entries_.find(id);
        if (it == entries_.end()) return Status::NotFound;

        auto& entry = it->second;
        if (entry.state != ComponentState::Running) {
            return Status::Error;
        }

        transition(id, entry, ComponentState::Running, ComponentState::Paused, "paused");
        return Status::Ok;
    }

    [[nodiscard]] Status resume(const ComponentId& id) override {
        return start(id); // start handles Paused → Running
    }

    [[nodiscard]] Status stop(const ComponentId& id) override {
        std::lock_guard lock(mutex_);

        auto it = entries_.find(id);
        if (it == entries_.end()) return Status::NotFound;

        auto& entry = it->second;
        if (entry.state != ComponentState::Running &&
            entry.state != ComponentState::Paused &&
            entry.state != ComponentState::Error) {
            return Status::Error;
        }

        auto old_state = entry.state;
        transition(id, entry, old_state, ComponentState::Stopping, "stopping");

        auto result = entry.component->stop();
        transition(id, entry, ComponentState::Stopping, ComponentState::Stopped,
                   result == Status::Ok ? "stopped cleanly" : "stopped with errors");

        return result;
    }

    [[nodiscard]] Status initialize_all(const Config& config) override {
        // Collect IDs first to avoid holding lock during init
        std::vector<ComponentId> ids;
        {
            std::lock_guard lock(mutex_);
            for (const auto& [id, _] : entries_) {
                ids.push_back(id);
            }
        }

        Status overall = Status::Ok;
        for (const auto& id : ids) {
            auto result = initialize(id, config);
            if (result != Status::Ok) {
                UADOS_LOG_ERROR("Failed to initialize component '{}'", id);
                overall = Status::Error;
            }
        }
        return overall;
    }

    [[nodiscard]] Status start_all() override {
        std::vector<ComponentId> ids;
        {
            std::lock_guard lock(mutex_);
            for (const auto& [id, entry] : entries_) {
                if (entry.state == ComponentState::Initialized) {
                    ids.push_back(id);
                }
            }
        }

        Status overall = Status::Ok;
        for (const auto& id : ids) {
            auto result = start(id);
            if (result != Status::Ok) {
                UADOS_LOG_ERROR("Failed to start component '{}'", id);
                overall = Status::Error;
            }
        }
        return overall;
    }

    [[nodiscard]] Status stop_all() override {
        std::vector<ComponentId> ids;
        {
            std::lock_guard lock(mutex_);
            for (const auto& [id, entry] : entries_) {
                if (entry.state == ComponentState::Running ||
                    entry.state == ComponentState::Paused) {
                    ids.push_back(id);
                }
            }
        }

        // Stop in reverse registration order
        std::reverse(ids.begin(), ids.end());

        Status overall = Status::Ok;
        for (const auto& id : ids) {
            auto result = stop(id);
            if (result != Status::Ok) {
                overall = Status::Error;
            }
        }
        return overall;
    }

    [[nodiscard]] ComponentState get_state(const ComponentId& id) const override {
        std::lock_guard lock(mutex_);
        auto it = entries_.find(id);
        if (it == entries_.end()) return ComponentState::Error;
        return it->second.state;
    }

    [[nodiscard]] std::vector<std::pair<ComponentId, ComponentState>>
    all_states() const override {
        std::lock_guard lock(mutex_);
        std::vector<std::pair<ComponentId, ComponentState>> result;
        result.reserve(entries_.size());
        for (const auto& [id, entry] : entries_) {
            result.emplace_back(id, entry.state);
        }
        return result;
    }

    void on_lifecycle_event(LifecycleCallback callback) override {
        std::lock_guard lock(mutex_);
        callbacks_.push_back(std::move(callback));
    }

private:
    struct Entry {
        std::shared_ptr<IComponent> component;
        ComponentState state{ComponentState::Loaded};
    };

    void transition(const ComponentId& id, Entry& entry,
                    ComponentState old_state, ComponentState new_state,
                    const std::string& reason) {
        entry.state = new_state;

        LifecycleEvent event{
            .component_id = id,
            .old_state = old_state,
            .new_state = new_state,
            .timestamp = Clock::now(),
            .reason = reason
        };

        UADOS_LOG_INFO("Component '{}': {} → {} ({})",
                       id, static_cast<int>(old_state),
                       static_cast<int>(new_state), reason);

        for (auto& callback : callbacks_) {
            try {
                callback(event);
            } catch (const std::exception& e) {
                UADOS_LOG_ERROR("Lifecycle callback threw: {}", e.what());
            }
        }
    }

    mutable std::mutex mutex_;
    std::unordered_map<ComponentId, Entry> entries_;
    std::vector<LifecycleCallback> callbacks_;
};

/// Factory function
std::unique_ptr<ILifecycleManager> create_lifecycle_manager() {
    return std::make_unique<LifecycleManagerImpl>();
}

} // namespace uados::core
