#pragma once

/// @file event_bus_factory.hpp
/// @brief Factory function for creating event bus instances.

#include "uados/event_bus/event_bus.hpp"
#include <memory>

namespace uados::core {

/// Create a new event bus instance
std::unique_ptr<IEventBus> create_event_bus();

} // namespace uados::core
