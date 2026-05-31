#include "uados/resource_profiler.hpp"

#include <gtest/gtest.h>
#include <thread>

using namespace uados::common;

TEST(HardeningProfilerTest, TestTimingLatencyMeasurement) {
    ResourceProfiler profiler;

    profiler.start_timer();
    
    // Sleep for 10ms to simulate a work loop cycle
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    double latency = profiler.stop_timer();

    // Latency should be at least 10ms (0.010s)
    EXPECT_GE(latency, 0.009);
    EXPECT_LT(latency, 0.200); // upper limit buffer
}

TEST(HardeningProfilerTest, TestTimingJitterCalculation) {
    ResourceProfiler profiler;

    double expected_period = 0.010; // 10ms

    // Simulated loops: 10ms, 11ms, 9ms, 10ms, 12ms -> deviations are: 0ms, 1ms, 1ms, 0ms, 2ms
    // Average Jitter should be: (0 + 1 + 1 + 0 + 2) / 5 = 4 / 5 = 0.8ms (0.0008s)
    std::vector<double> actuals = {0.010, 0.011, 0.009, 0.010, 0.012};

    double jitter = profiler.get_jitter(expected_period, actuals);

    EXPECT_NEAR(jitter, 0.0008, 0.00001);
}

TEST(HardeningProfilerTest, TestMemoryLeakfootprintAudits) {
    ResourceProfiler profiler;

    bool leak_detected = false;
    size_t memory = profiler.audit_leak_stats(leak_detected);

    // Initial stable footprint is 16MB
    EXPECT_EQ(memory, 16 * 1024 * 1024);
    EXPECT_FALSE(leak_detected);

    // Simulate prolonged executions (inject 110 calls -> should trigger leak detection)
    for (int i = 0; i < 110; ++i) {
        memory = profiler.audit_leak_stats(leak_detected);
    }

    EXPECT_TRUE(leak_detected);
    EXPECT_GT(memory, 16 * 1024 * 1024); // Memory grown due to leak
}
