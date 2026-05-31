#pragma once

/// @file version.hpp
/// @brief UADOS version information and compile-time metadata.

#include <cstdint>
#include <string_view>

namespace uados {

/// Semantic version components
struct Version {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;

    [[nodiscard]] constexpr bool operator==(const Version& other) const noexcept = default;
    [[nodiscard]] constexpr auto operator<=>(const Version& other) const noexcept = default;
};

/// Current UADOS version
inline constexpr Version kVersion{0, 1, 0};

/// Version as string
inline constexpr std::string_view kVersionString = "0.1.0";

/// Project name
inline constexpr std::string_view kProjectName = "UADOS";

/// Full project identifier
inline constexpr std::string_view kProjectFullName =
    "Universal Autonomous Driving Operating System";

} // namespace uados
