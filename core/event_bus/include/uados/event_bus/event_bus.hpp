#pragma once

/// @file event_bus.hpp
/// @brief Zero-copy publish-subscribe event bus interface.
///
/// The event bus is the central communication backbone of UADOS.
/// It provides topic-based publish-subscribe messaging with zero-copy
/// shared memory for high-throughput sensor data paths.

#include "uados/types.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uados::core {

// ============================================================================
// Message Types
// ============================================================================

/// Base message type for all event bus messages
struct Message {
    virtual ~Message() = default;

    Timestamp timestamp{Clock::now()};  ///< Message creation time
    std::string source;                  ///< Source component name
    uint64_t sequence{0};               ///< Monotonic sequence number
};

/// Shared pointer to const message (zero-copy reference)
template <typename T = Message>
using MessagePtr = std::shared_ptr<const T>;

/// Typed message wrapper
template <typename PayloadT>
struct TypedMessage : public Message {
    PayloadT payload;

    explicit TypedMessage(PayloadT p) : payload(std::move(p)) {}
};

// ============================================================================
// Quality of Service
// ============================================================================

/// Quality of Service policy for subscriptions
enum class QoSPolicy : uint8_t {
    /// Best-effort delivery; messages may be dropped under load
    BestEffort,

    /// Reliable delivery; messages are queued and never dropped
    Reliable,

    /// Last-value cache; subscriber receives the latest message on connect
    LastValue,

    /// Batched delivery; messages are collected and delivered in batches
    Batched
};

/// Subscription configuration
struct SubscriptionConfig {
    QoSPolicy qos{QoSPolicy::BestEffort};
    size_t queue_depth{64};             ///< Maximum pending messages
    Duration max_latency{Duration::zero()}; ///< Maximum delivery latency
};

// ============================================================================
// Event Bus Interface
// ============================================================================

/// Callback type for message delivery
using MessageCallback = std::function<void(MessagePtr<Message>)>;

/// Topic statistics
struct TopicStats {
    std::string topic_name;
    size_t subscriber_count{0};
    uint64_t total_published{0};
    uint64_t total_delivered{0};
    uint64_t total_dropped{0};
    Duration avg_latency{};
    Duration max_latency{};
};

/// @brief Abstract event bus interface.
///
/// The event bus supports:
/// - Topic-based publish-subscribe
/// - Zero-copy message passing (shared_ptr semantics)
/// - Configurable QoS per subscription
/// - Topic statistics and monitoring
class IEventBus {
public:
    virtual ~IEventBus() = default;

    /// Publish a message to a topic
    /// @param topic Topic name (hierarchical, e.g., "sensor/camera/0")
    /// @param msg Shared pointer to message (zero-copy)
    virtual void publish(std::string_view topic, MessagePtr<Message> msg) = 0;

    /// Subscribe to a topic
    /// @param topic Topic name or wildcard pattern
    /// @param callback Function called on message delivery
    /// @param config Subscription configuration
    /// @return Unique subscription ID for unsubscribe
    [[nodiscard]] virtual SubscriptionId subscribe(
        std::string_view topic,
        MessageCallback callback,
        SubscriptionConfig config = {}) = 0;

    /// Unsubscribe from a topic
    /// @param id Subscription ID returned by subscribe()
    virtual void unsubscribe(SubscriptionId id) = 0;

    /// List all active topics
    [[nodiscard]] virtual std::vector<std::string> list_topics() const = 0;

    /// Get subscriber count for a topic
    [[nodiscard]] virtual size_t subscriber_count(std::string_view topic) const = 0;

    /// Get statistics for a topic
    [[nodiscard]] virtual TopicStats topic_stats(std::string_view topic) const = 0;

    /// Get statistics for all topics
    [[nodiscard]] virtual std::vector<TopicStats> all_stats() const = 0;

    // -- Typed helpers --

    /// Publish a typed message
    template <typename T>
    void publish_typed(std::string_view topic, T payload, const std::string& source = "") {
        auto msg = std::make_shared<TypedMessage<T>>(std::move(payload));
        msg->source = source;
        publish(topic, msg);
    }

    /// Subscribe with typed callback
    template <typename T>
    SubscriptionId subscribe_typed(
        std::string_view topic,
        std::function<void(const T&, const Message&)> callback,
        SubscriptionConfig config = {})
    {
        return subscribe(topic, [cb = std::move(callback)](MessagePtr<Message> msg) {
            if (auto typed = std::dynamic_pointer_cast<const TypedMessage<T>>(msg)) {
                cb(typed->payload, *typed);
            }
        }, config);
    }
};

} // namespace uados::core
