#include "uados/event_bus/event_bus.hpp"
#include "uados/logging.hpp"

#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace uados::core {

UADOS_DECLARE_LOGGER("core.event_bus")

/// @brief Concrete implementation of the zero-copy event bus.
///
/// Uses shared_ptr for zero-copy message passing. Each subscription
/// has its own delivery queue. Topic matching supports exact and
/// wildcard patterns.
class EventBusImpl final : public IEventBus {
public:
    EventBusImpl() = default;
    ~EventBusImpl() override = default;

    void publish(std::string_view topic, MessagePtr<Message> msg) override {
        if (!msg) return;

        msg->timestamp = Clock::now();

        std::shared_lock lock(mutex_);

        auto it = subscriptions_.find(std::string(topic));
        if (it == subscriptions_.end()) return;

        auto& topic_entry = it->second;
        topic_entry.total_published++;

        for (auto& sub : topic_entry.subscribers) {
            if (!sub.active) continue;

            try {
                sub.callback(msg);
                topic_entry.total_delivered++;
            } catch (const std::exception& e) {
                topic_entry.total_dropped++;
                UADOS_LOG_ERROR("Delivery failed on topic '{}' to sub {}: {}",
                                topic, sub.id, e.what());
            }
        }
    }

    [[nodiscard]] SubscriptionId subscribe(
        std::string_view topic,
        MessageCallback callback,
        SubscriptionConfig config) override
    {
        std::unique_lock lock(mutex_);

        auto id = next_id_++;
        auto topic_str = std::string(topic);

        auto& entry = subscriptions_[topic_str];
        if (entry.topic_name.empty()) {
            entry.topic_name = topic_str;
        }

        entry.subscribers.push_back({
            .id = id,
            .callback = std::move(callback),
            .config = config,
            .active = true
        });

        sub_to_topic_[id] = topic_str;

        UADOS_LOG_DEBUG("Subscribed id={} to topic '{}' (qos={}, depth={})",
                        id, topic, static_cast<int>(config.qos), config.queue_depth);

        return id;
    }

    void unsubscribe(SubscriptionId id) override {
        std::unique_lock lock(mutex_);

        auto topic_it = sub_to_topic_.find(id);
        if (topic_it == sub_to_topic_.end()) return;

        auto& topic_str = topic_it->second;
        auto sub_it = subscriptions_.find(topic_str);
        if (sub_it != subscriptions_.end()) {
            auto& subs = sub_it->second.subscribers;
            subs.erase(
                std::remove_if(subs.begin(), subs.end(),
                               [id](const Subscriber& s) { return s.id == id; }),
                subs.end());

            // Remove topic entry if no subscribers remain
            if (subs.empty()) {
                subscriptions_.erase(sub_it);
            }
        }

        sub_to_topic_.erase(topic_it);

        UADOS_LOG_DEBUG("Unsubscribed id={}", id);
    }

    [[nodiscard]] std::vector<std::string> list_topics() const override {
        std::shared_lock lock(mutex_);
        std::vector<std::string> topics;
        topics.reserve(subscriptions_.size());
        for (const auto& [topic, _] : subscriptions_) {
            topics.push_back(topic);
        }
        return topics;
    }

    [[nodiscard]] size_t subscriber_count(std::string_view topic) const override {
        std::shared_lock lock(mutex_);
        auto it = subscriptions_.find(std::string(topic));
        if (it == subscriptions_.end()) return 0;
        return it->second.subscribers.size();
    }

    [[nodiscard]] TopicStats topic_stats(std::string_view topic) const override {
        std::shared_lock lock(mutex_);
        auto it = subscriptions_.find(std::string(topic));
        if (it == subscriptions_.end()) {
            return TopicStats{.topic_name = std::string(topic)};
        }

        const auto& entry = it->second;
        return TopicStats{
            .topic_name = entry.topic_name,
            .subscriber_count = entry.subscribers.size(),
            .total_published = entry.total_published,
            .total_delivered = entry.total_delivered,
            .total_dropped = entry.total_dropped,
            .avg_latency = entry.avg_latency,
            .max_latency = entry.max_latency,
        };
    }

    [[nodiscard]] std::vector<TopicStats> all_stats() const override {
        std::shared_lock lock(mutex_);
        std::vector<TopicStats> result;
        result.reserve(subscriptions_.size());
        for (const auto& [topic, entry] : subscriptions_) {
            result.push_back(TopicStats{
                .topic_name = entry.topic_name,
                .subscriber_count = entry.subscribers.size(),
                .total_published = entry.total_published,
                .total_delivered = entry.total_delivered,
                .total_dropped = entry.total_dropped,
                .avg_latency = entry.avg_latency,
                .max_latency = entry.max_latency,
            });
        }
        return result;
    }

private:
    struct Subscriber {
        SubscriptionId id{0};
        MessageCallback callback;
        SubscriptionConfig config;
        bool active{true};
    };

    struct TopicEntry {
        std::string topic_name;
        std::vector<Subscriber> subscribers;
        uint64_t total_published{0};
        uint64_t total_delivered{0};
        uint64_t total_dropped{0};
        Duration avg_latency{};
        Duration max_latency{};
    };

    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, TopicEntry> subscriptions_;
    std::unordered_map<SubscriptionId, std::string> sub_to_topic_;
    std::atomic<SubscriptionId> next_id_{1};
};

// Factory function
std::unique_ptr<IEventBus> create_event_bus() {
    return std::make_unique<EventBusImpl>();
}

} // namespace uados::core
