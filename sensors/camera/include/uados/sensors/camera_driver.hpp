#pragma once

/// @file camera_driver.hpp
/// @brief Camera sensor driver.

#include "uados/sensors/sensor.hpp"
#include <mutex>
#include <string>

namespace uados::sensors {

/// @brief Camera sensor driver for image acquisition.
///
/// Under hardware configurations, interfaces with standard industrial USB/GigE
/// cameras. In mock configurations, outputs synthetic frames (patterns).
class CameraDriver final : public ISensor {
public:
    CameraDriver() = default;
    ~CameraDriver() override;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "sensors.camera"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- ISensor --
    [[nodiscard]] std::shared_ptr<SensorData> read() override;
    [[nodiscard]] SensorType type() const override { return SensorType::Camera; }
    [[nodiscard]] std::string_view sensor_id() const override { return sensor_id_; }
    [[nodiscard]] Extrinsics extrinsics() const override;
    void set_extrinsics(const Extrinsics& ext) override;
    [[nodiscard]] SensorHealth sensor_health() const override;
    [[nodiscard]] bool is_active() const override;

private:
    mutable std::mutex mutex_;
    std::string sensor_id_{"camera_front"};
    Extrinsics extrinsics_;
    SensorHealth health_;
    bool active_{false};

    uint32_t width_{640};
    uint32_t height_{480};
    uint64_t seq_{0};
};

} // namespace uados::sensors
