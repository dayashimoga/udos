#include "uados/kernel/kernel.hpp"
#include "uados/kernel/config_manager.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

using namespace uados::core;

class KernelIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a dummy config file
        config_path_ = "test_config.yaml";
        std::ofstream out(config_path_);
        out << "scheduler:\n"
            << "  tick_interval_ms: 2\n"
            << "health:\n"
            << "  watchdog_period_ms: 50\n"
            << "components:\n"
            << "  perception.detection:\n"
            << "    enabled: true\n"
            << "    model: yolov8\n"
            << "  control.steering:\n"
            << "    kp: 1.5\n";
        out.close();
    }

    void TearDown() override {
        if (std::filesystem::exists(config_path_)) {
            std::filesystem::remove(config_path_);
        }
    }

    std::string config_path_;
};

TEST_F(KernelIntegrationTest, ConfigManagerLoading) {
    auto config_mgr = create_config_manager();
    ASSERT_EQ(config_mgr->load_file("nonexistent.yaml"), Status::NotFound);
    ASSERT_EQ(config_mgr->load_file(config_path_), Status::Ok);

    auto sys_node = config_mgr->get_system_config("scheduler");
    ASSERT_TRUE(sys_node);
    EXPECT_EQ(sys_node["tick_interval_ms"].as<int>(), 2);

    auto comp_node = config_mgr->get_component_config("perception.detection");
    ASSERT_TRUE(comp_node);
    EXPECT_TRUE(comp_node["enabled"].as<bool>());
    EXPECT_EQ(comp_node["model"].as<std::string>(), "yolov8");
}

TEST_F(KernelIntegrationTest, KernelInitializationAndShutdown) {
    auto kernel = create_kernel();
    ASSERT_EQ(kernel->init(config_path_), Status::Ok);

    // Verify sub-components are accessible
    auto& scheduler = kernel->scheduler();
    EXPECT_FALSE(scheduler.is_running());

    auto& config_mgr = kernel->config_manager();
    auto health_node = config_mgr.get_system_config("health");
    EXPECT_EQ(health_node["watchdog_period_ms"].as<int>(), 50);

    // Run kernel in a background thread and shut it down
    std::thread run_thread([&kernel]() {
        kernel->run();
    });

    // Wait a brief moment for startup
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Request shutdown
    kernel->shutdown();
    run_thread.join();

    EXPECT_FALSE(scheduler.is_running());
}
