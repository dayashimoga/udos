#pragma once

/// @file ota_manager.hpp
/// @brief Over-The-Air software update package manager.

#include "uados/component.hpp"
#include "uados/types.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace uados::fleet {

/// @brief Over-The-Air Update Manager component.
///
/// Handles receiving, verifying, rollouts, and rollback recovery for software updates,
/// validating SemVer guidelines and checksum packet signatures.
class OTAManager final : public uados::core::ComponentBase {
public:
    OTAManager() = default;
    ~OTAManager() override = default;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "fleet.ota"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    /// Receives, validates, and rolls out an incoming software update package
    /// @param package_name Target package label
    /// @param version Package SemVer string (e.g., "0.2.0")
    /// @param expected_checksum Verification checksum signature
    /// @param binary_payload Shared library mock data payload
    /// @return Status::Ok on successful hot-update rollout
    [[nodiscard]] Status process_ota_update(
        const std::string& package_name,
        const std::string& version,
        const std::string& expected_checksum,
        const std::string& binary_payload) noexcept;

    /// Query active system version
    [[nodiscard]] std::string get_active_version() const noexcept { return active_version_; }

    /// Query total count of rolled-back operations
    [[nodiscard]] int get_rollback_count() const noexcept { return rollback_count_; }

    /// Reset rollback count metrics
    void reset_metrics() noexcept;

private:
    mutable std::mutex mutex_;
    bool active_{false};

    std::string active_version_{"0.1.0"};
    int rollback_count_{0};

    [[nodiscard]] bool verify_semver(const std::string& current, const std::string& incoming) const noexcept;
    [[nodiscard]] bool verify_checksum(const std::string& binary_payload, const std::string& expected_checksum) const noexcept;
};

} // namespace uados::fleet
