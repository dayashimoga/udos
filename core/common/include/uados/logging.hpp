#pragma once

/// @file logging.hpp
/// @brief UADOS structured logging macros and configuration.
///
/// Provides a unified logging interface wrapping spdlog with
/// component-aware structured logging. All log entries include
/// the component name and thread ID for traceability.

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#include <memory>
#include <string>
#include <string_view>

namespace uados::logging {

/// Initialize the global logging system
/// @param level Minimum log level (trace, debug, info, warn, error, critical)
/// @param pattern Log pattern (spdlog format)
inline void init(
    spdlog::level::level_enum level = spdlog::level::info,
    const std::string& pattern = "[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] [%n] [tid:%t] %v")
{
    spdlog::set_pattern(pattern);
    spdlog::set_level(level);
    spdlog::flush_every(std::chrono::seconds(1));
}

/// Create a named logger for a component
/// @param name Component name (e.g., "kernel", "perception.detection")
/// @return Shared pointer to the logger
inline std::shared_ptr<spdlog::logger> create_logger(const std::string& name) {
    auto logger = spdlog::get(name);
    if (!logger) {
        logger = spdlog::stdout_color_mt(name);
    }
    return logger;
}

/// Convenience macro for declaring a component-local logger
#define UADOS_DECLARE_LOGGER(name) \
    static inline auto& logger() { \
        static auto log = ::uados::logging::create_logger(name); \
        return log; \
    }

} // namespace uados::logging

// ============================================================================
// Logging Macros
// ============================================================================
// Usage:
//   UADOS_DECLARE_LOGGER("perception.detection")
//   UADOS_LOG_INFO("Detected {} objects in {}ms", count, elapsed);

#define UADOS_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(logger(), __VA_ARGS__)
#define UADOS_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(logger(), __VA_ARGS__)
#define UADOS_LOG_INFO(...)     SPDLOG_LOGGER_INFO(logger(), __VA_ARGS__)
#define UADOS_LOG_WARN(...)     SPDLOG_LOGGER_WARN(logger(), __VA_ARGS__)
#define UADOS_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(logger(), __VA_ARGS__)
#define UADOS_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(logger(), __VA_ARGS__)
