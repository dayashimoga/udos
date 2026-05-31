#include "uados/lifecycle/lifecycle_manager.hpp"
#include "uados/component.hpp"
#include <gtest/gtest.h>
#include <atomic>

using namespace uados::core;

class MockComponent : public ComponentBase {
public:
    Status init(const Config&) override { return init_status; }
    Status start() override { return start_status; }
    Status stop() override { return stop_status; }
    std::string_view name() const override { return "mock"; }
    Version version() const override { return {1, 0, 0}; }

    Status init_status{Status::Ok};
    Status start_status{Status::Ok};
    Status stop_status{Status::Ok};
};

TEST(LifecycleTest, StateTransitions) {
    auto manager = create_lifecycle_manager();
    auto component = std::make_shared<MockComponent>();
    ComponentId id = "mock_component";

    manager->register_component(id, component);
    EXPECT_EQ(manager->get_state(id), ComponentState::Loaded);

    Config conf;
    // Loaded -> Initialized
    EXPECT_EQ(manager->initialize(id, conf), Status::Ok);
    EXPECT_EQ(manager->get_state(id), ComponentState::Initialized);

    // Initialized -> Running
    EXPECT_EQ(manager->start(id), Status::Ok);
    EXPECT_EQ(manager->get_state(id), ComponentState::Running);

    // Running -> Stopped
    EXPECT_EQ(manager->stop(id), Status::Ok);
    EXPECT_EQ(manager->get_state(id), ComponentState::Stopped);
}

TEST(LifecycleTest, ErrorTransitions) {
    auto manager = create_lifecycle_manager();
    auto component = std::make_shared<MockComponent>();
    ComponentId id = "mock_component";

    component->init_status = Status::Error; // Fail on init
    manager->register_component(id, component);

    Config conf;
    EXPECT_EQ(manager->initialize(id, conf), Status::Error);
    EXPECT_EQ(manager->get_state(id), ComponentState::Error);
}

TEST(LifecycleTest, BatchOperations) {
    auto manager = create_lifecycle_manager();
    auto c1 = std::make_shared<MockComponent>();
    auto c2 = std::make_shared<MockComponent>();

    manager->register_component("c1", c1);
    manager->register_component("c2", c2);

    Config conf;
    EXPECT_EQ(manager->initialize_all(conf), Status::Ok);
    EXPECT_EQ(manager->get_state("c1"), ComponentState::Initialized);
    EXPECT_EQ(manager->get_state("c2"), ComponentState::Initialized);

    EXPECT_EQ(manager->start_all(), Status::Ok);
    EXPECT_EQ(manager->get_state("c1"), ComponentState::Running);
    EXPECT_EQ(manager->get_state("c2"), ComponentState::Running);

    EXPECT_EQ(manager->stop_all(), Status::Ok);
    EXPECT_EQ(manager->get_state("c1"), ComponentState::Stopped);
    EXPECT_EQ(manager->get_state("c2"), ComponentState::Stopped);
}
