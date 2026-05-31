#include "uados/kernel/spsc_queue.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>

using namespace uados::core;

TEST(SPSCQueueTest, BasicOperations) {
    SPSCQueue<int, 4> queue; // Capacity is 4, but capacity() is 3 due to full detection slot

    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.full());
    EXPECT_EQ(queue.capacity(), 3);
    EXPECT_EQ(queue.size_approx(), 0);

    EXPECT_TRUE(queue.try_push(10));
    EXPECT_TRUE(queue.try_push(20));
    EXPECT_TRUE(queue.try_push(30));
    EXPECT_FALSE(queue.try_push(40)); // should fail, full

    EXPECT_FALSE(queue.empty());
    EXPECT_TRUE(queue.full());
    EXPECT_EQ(queue.size_approx(), 3);

    auto val = queue.try_pop();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 10);

    val = queue.try_pop();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 20);

    val = queue.try_pop();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 30);

    val = queue.try_pop();
    EXPECT_FALSE(val.has_value()); // empty
}

TEST(SPSCQueueTest, MoveSemantics) {
    struct UniquePayload {
        std::unique_ptr<int> ptr;
        UniquePayload() = default;
        explicit UniquePayload(int v) : ptr(std::make_unique<int>(v)) {}
    };

    SPSCQueue<UniquePayload, 4> queue;
    EXPECT_TRUE(queue.try_push(UniquePayload(42)));

    auto popped = queue.try_pop();
    ASSERT_TRUE(popped.has_value());
    ASSERT_NE(popped->ptr, nullptr);
    EXPECT_EQ(*(popped->ptr), 42);
}

TEST(SPSCQueueTest, ConcurrentProducerConsumer) {
    SPSCQueue<int, 1024> queue;
    const int kNumElements = 10000;
    std::atomic<bool> producer_done{false};

    // Spawn consumer thread
    std::thread consumer([&queue, &producer_done, kNumElements]() {
        int expected = 0;
        int popped_count = 0;
        while (popped_count < kNumElements) {
            auto val = queue.try_pop();
            if (val.has_value()) {
                EXPECT_EQ(*val, expected);
                expected++;
                popped_count++;
            } else {
                std::this_thread::yield();
            }
        }
    });

    // Producer (main thread)
    for (int i = 0; i < kNumElements; ++i) {
        while (!queue.try_push(i)) {
            std::this_thread::yield();
        }
    }
    producer_done = true;

    consumer.join();
    EXPECT_TRUE(queue.empty());
}
