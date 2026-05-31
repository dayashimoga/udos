#include "uados/kernel/memory_pool.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>

using namespace uados::core;

struct TestPayload {
    int x;
    double y;
    char buffer[128];
};

TEST(MemoryPoolTest, BasicAllocation) {
    MemoryPool pool(sizeof(TestPayload), 4);

    EXPECT_EQ(pool.block_count(), 4);
    EXPECT_EQ(pool.allocated(), 0);
    EXPECT_EQ(pool.available(), 4);

    void* p1 = pool.allocate();
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(pool.allocated(), 1);
    EXPECT_EQ(pool.available(), 3);
    EXPECT_TRUE(pool.owns(p1));

    void* p2 = pool.allocate();
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(pool.allocated(), 2);
    EXPECT_EQ(pool.available(), 2);
    EXPECT_TRUE(pool.owns(p2));

    pool.deallocate(p1);
    EXPECT_EQ(pool.allocated(), 1);
    EXPECT_EQ(pool.available(), 3);

    pool.deallocate(p2);
    EXPECT_EQ(pool.allocated(), 0);
    EXPECT_EQ(pool.available(), 4);
}

TEST(MemoryPoolTest, PoolExhaustion) {
    MemoryPool pool(sizeof(TestPayload), 2);

    void* p1 = pool.allocate();
    void* p2 = pool.allocate();
    EXPECT_NE(p1, nullptr);
    EXPECT_NE(p2, nullptr);

    void* p3 = pool.allocate();
    EXPECT_EQ(p3, nullptr); // Exhausted

    pool.deallocate(p1);
    p3 = pool.allocate();
    EXPECT_NE(p3, nullptr); // Recovered
    pool.deallocate(p2);
    pool.deallocate(p3);
}

TEST(MemoryPoolTest, ConstructDestroy) {
    struct Constructible {
        int val;
        static int active_count;
        Constructible(int v) : val(v) { active_count++; }
        ~Constructible() { active_count--; }
    };

    int Constructible::active_count = 0;

    {
        MemoryPool pool(sizeof(Constructible), 2);
        Constructible* obj = pool.construct<Constructible>(42);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(obj->val, 42);
        EXPECT_EQ(Constructible::active_count, 1);

        pool.destroy(obj);
        EXPECT_EQ(Constructible::active_count, 0);
    }
}

TEST(MemoryPoolTest, ConcurrentAllocation) {
    MemoryPool pool(64, 1000);
    std::vector<std::thread> threads;
    const int kNumThreads = 8;
    const int kAllocationsPerThread = 100;

    for (int t = 0; t < kNumThreads; ++t) {
        threads.emplace_back([&pool]() {
            std::vector<void*> allocated;
            allocated.reserve(kAllocationsPerThread);

            for (int i = 0; i < kAllocationsPerThread; ++i) {
                void* ptr = pool.allocate();
                if (ptr) {
                    allocated.push_back(ptr);
                }
            }

            for (void* ptr : allocated) {
                pool.deallocate(ptr);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(pool.allocated(), 0);
}

TEST(PoolAllocatorTest, MultiSizeAllocation) {
    std::vector<PoolAllocator::PoolTier> tiers = {
        {64, 10},
        {256, 5}
    };
    PoolAllocator allocator(tiers);

    void* p1 = allocator.allocate(32); // should go to 64 tier
    void* p2 = allocator.allocate(128); // should go to 256 tier

    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);

    auto stats = allocator.stats();
    ASSERT_EQ(stats.size(), 2);
    EXPECT_EQ(stats[0].block_size, 64);
    EXPECT_EQ(stats[0].allocated, 1);
    EXPECT_EQ(stats[1].block_size, 256);
    EXPECT_EQ(stats[1].allocated, 1);

    allocator.deallocate(p1);
    allocator.deallocate(p2);
}
