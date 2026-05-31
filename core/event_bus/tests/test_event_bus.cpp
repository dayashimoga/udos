#include "uados/event_bus/event_bus.hpp"
#include "uados/event_bus/event_bus_factory.hpp"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace uados::core;

struct SimpleData {
    int value;
};

TEST(EventBusTest, BasicPubSub) {
    auto bus = create_event_bus();

    std::atomic<int> received_value{0};
    std::atomic<int> callback_count{0};

    auto sub_id = bus->subscribe("sensor/camera", [&](MessagePtr<Message> msg) {
        auto typed = std::dynamic_pointer_cast<const TypedMessage<SimpleData>>(msg);
        if (typed) {
            received_value = typed->payload.value;
            callback_count++;
        }
    });

    EXPECT_NE(sub_id, 0);
    EXPECT_EQ(bus->subscriber_count("sensor/camera"), 1);

    bus->publish_typed<SimpleData>("sensor/camera", SimpleData{100}, "test_source");

    // Wait for delivery
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_EQ(callback_count, 1);
    EXPECT_EQ(received_value, 100);

    // Unsubscribe
    bus->unsubscribe(sub_id);
    EXPECT_EQ(bus->subscriber_count("sensor/camera"), 0);

    bus->publish_typed<SimpleData>("sensor/camera", SimpleData{200}, "test_source");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Callback count should still be 1
    EXPECT_EQ(callback_count, 1);
}

TEST(EventBusTest, TypedPubSubHelper) {
    auto bus = create_event_bus();
    std::atomic<int> received_value{0};

    bus->subscribe_typed<SimpleData>("sensor/radar", [&](const SimpleData& data, const Message& msg) {
        received_value = data.value;
        EXPECT_EQ(msg.source, "radar_driver");
    });

    bus->publish_typed<SimpleData>("sensor/radar", SimpleData{42}, "radar_driver");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(received_value, 42);
}

TEST(EventBusTest, Statistics) {
    auto bus = create_event_bus();

    bus->subscribe("control/steering", [](MessagePtr<Message>) {});
    bus->subscribe("control/steering", [](MessagePtr<Message>) {});

    EXPECT_EQ(bus->subscriber_count("control/steering"), 2);

    bus->publish_typed<int>("control/steering", 10);
    bus->publish_typed<int>("control/steering", 20);

    auto stats = bus->topic_stats("control/steering");
    EXPECT_EQ(stats.topic_name, "control/steering");
    EXPECT_EQ(stats.subscriber_count, 2);
    EXPECT_EQ(stats.total_published, 2);
    EXPECT_EQ(stats.total_delivered, 4); // 2 publishes * 2 subscribers
}
