#pragma once

/// @file lidar_driver.hpp
/// @brief LiDAR sensor driver.

#include "uados/sensors/sensor.hpp"
#include <mutex>
#include <string>

namespace uados::sensors {

/// @brief LiDAR sensor driver for point-cloud acquisition.
class LiDARDriver final : public ISensor {
public:
    LiDARDriver() = default;
    ~LiDARDriver() override;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "sensors.lidar"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- ISensor --
    [[nodiscard]] std::shared_ptr<SensorData> read() override;
    [[nodiscard]] SensorType type() const override { return SensorType::LiDAR; }
    [[nodiscard]] std::string_view sensor_id() const override { return sensor_id_; }
    [[nodiscard]] Extrinsics extrinsics() const override;
    void set_extrinsics(const Extrinsics& ext) override;
    [[nodiscard]] SensorHealth sensor_health() const override;
    [[nodiscard]] bool is_active() const override;

private:
    mutable std::mutex mutex_;
    std::string sensor_id_{"lidar_top"};
    Extrinsics extrinsics_;
    SensorHealth health_;
    bool active_{false};

    uint32_t channels_{16}; // 16-channel LiDAR (e.g. Velodyne VLP-16)
    uint32_t points_per_channel_{180}; // 180 horizontal steps (2-degree resolution)
    uint64_t seq_{0};
};

} // namespace uados::sensors
