#include "uados/scheduler/scheduler.hpp"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace uados::core;

TEST(SchedulerTest, PeriodicTaskExecution) {
    auto scheduler = create_scheduler(std::chrono::milliseconds(1));

    std::atomic<int> execution_count{0};
    TaskConfig config{
        .name = "test_task",
        .function = [&]() { execution_count++; },
        .priority = Priority::Normal,
        .period = std::chrono::milliseconds(5),
        .deadline = std::chrono::milliseconds(2),
        .enabled = true
    };

    auto id = scheduler->register_task(config);
    EXPECT_NE(id, 0);

    // Run scheduler in background thread
    std::thread sched_thread([&]() {
        scheduler->run();
    });

    // Let it run for 25ms (should execute ~5 times)
    std::this_thread::sleep_for(std::chrono::milliseconds(28));

    scheduler->stop();
    sched_thread.join();

    EXPECT_GE(execution_count.load(), 4);
    EXPECT_LE(execution_count.load(), 7);

    auto stats = scheduler->task_stats(id);
    EXPECT_EQ(stats.id, id);
    EXPECT_EQ(stats.total_executions, execution_count.load());
    EXPECT_EQ(stats.deadline_misses, 0);
}

TEST(SchedulerTest, TaskEnableDisable) {
    auto scheduler = create_scheduler(std::chrono::milliseconds(1));

    std::atomic<int> execution_count{0};
    TaskConfig config{
        .name = "test_task",
        .function = [&]() { execution_count++; },
        .priority = Priority::Normal,
        .period = std::chrono::milliseconds(2),
        .enabled = false // Disabled initially
    };

    auto id = scheduler->register_task(config);

    std::thread sched_thread([&]() {
        scheduler->run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(execution_count.load(), 0); // shouldn't run

    // Enable task
    scheduler->set_enabled(id, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_GT(execution_count.load(), 0); // should run now

    scheduler->stop();
    sched_thread.join();
}

TEST(SchedulerTest, DeadlineMiss) {
    auto scheduler = create_scheduler(std::chrono::milliseconds(1));

    TaskConfig config{
        .name = "slow_task",
        .function = []() { std::this_thread::sleep_for(std::chrono::milliseconds(5)); },
        .priority = Priority::Normal,
        .period = std::chrono::milliseconds(10),
        .deadline = std::chrono::milliseconds(2), // very tight deadline
        .enabled = true
    };

    auto id = scheduler->register_task(config);

    std::thread sched_thread([&]() {
        scheduler->run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    scheduler->stop();
    sched_thread.join();

    auto stats = scheduler->task_stats(id);
    EXPECT_GT(stats.deadline_misses, 0);
    EXPECT_GT(scheduler->total_deadline_misses(), 0);
}
