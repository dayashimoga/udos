#include "uados/health/health_monitor.hpp"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace uados::core;

TEST(HealthMonitorTest, HeartbeatAndWatchdog) {
    auto monitor = create_health_monitor();

    ComponentId id = "perception.detection";
    Duration timeout = std::chrono::milliseconds(20);

    monitor->register_component(id, timeout);

    // Initial state should be healthy
    auto health = monitor->get_health(id);
    EXPECT_EQ(health.status, HealthStatus::Healthy);

    // Send heartbeat
    monitor->heartbeat(id);
    monitor->check_watchdogs();
    EXPECT_EQ(monitor->get_health(id).status, HealthStatus::Healthy);

    // Sleep past 1x timeout, should be degraded
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    monitor->check_watchdogs();
    EXPECT_EQ(monitor->get_health(id).status, HealthStatus::Degraded);

    // Sleep past 3x timeout, should be unhealthy
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    monitor->check_watchdogs();
    EXPECT_EQ(monitor->get_health(id).status, HealthStatus::Unhealthy);

    // Send heartbeat, should recover to healthy
    monitor->heartbeat(id);
    EXPECT_EQ(monitor->get_health(id).status, HealthStatus::Healthy);
}

TEST(HealthMonitorTest, ErrorReporting) {
    auto monitor = create_health_monitor();

    ComponentId id = "control.steering";
    monitor->register_component(id, std::chrono::milliseconds(100));

    monitor->report_error(id, "Actuator feedback timeout");
    EXPECT_EQ(monitor->get_health(id).status, HealthStatus::Unhealthy);
    EXPECT_EQ(monitor->get_health(id).last_error, "Actuator feedback timeout");

    // Clear by heartbeat
    monitor->heartbeat(id);
    EXPECT_EQ(monitor->get_health(id).status, HealthStatus::Healthy);
}

TEST(HealthMonitorTest, SystemHealthAggregation) {
    auto monitor = create_health_monitor();

    monitor->register_component("c1", std::chrono::milliseconds(10));
    monitor->register_component("c2", std::chrono::milliseconds(10));

    auto sys = monitor->system_health();
    EXPECT_EQ(sys.overall_status, HealthStatus::Healthy);
    EXPECT_EQ(sys.healthy_count, 2);

    monitor->report_error("c1", "Some error");
    sys = monitor->system_health();
    EXPECT_EQ(sys.overall_status, HealthStatus::Unhealthy);
    EXPECT_EQ(sys.healthy_count, 1);
    EXPECT_EQ(sys.unhealthy_count, 1);
}
