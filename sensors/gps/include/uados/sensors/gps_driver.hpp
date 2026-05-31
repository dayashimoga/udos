#pragma once

/// @file gps_driver.hpp
/// @brief GPS/GNSS sensor driver.

#include "uados/sensors/sensor.hpp"
#include <mutex>
#include <string>

namespace uados::sensors {

/// @brief GPS/GNSS sensor driver.
class GPSDriver final : public ISensor {
public:
    GPSDriver() = default;
    ~GPSDriver() override;

    // -- IComponent / ComponentBase --
    [[nodiscard]] Status init(const uados::core::Config& config) override;
    [[nodiscard]] Status start() override;
    [[nodiscard]] Status stop() override;
    [[nodiscard]] std::string_view name() const override { return "sensors.gps"; }
    [[nodiscard]] Version version() const override { return {0, 1, 0}; }

    // -- ISensor --
    [[nodiscard]] std::shared_ptr<SensorData> read() override;
    [[nodiscard]] SensorType type() const override { return SensorType::GPS; }
    [[nodiscard]] std::string_view sensor_id() const override { return sensor_id_; }
    [[nodiscard]] Extrinsics extrinsics() const override;
    void set_extrinsics(const Extrinsics& ext) override;
    [[nodiscard]] SensorHealth sensor_health() const override;
    [[nodiscard]] bool is_active() const override;

private:
    mutable std::mutex mutex_;
    std::string sensor_id_{"gps"};
    Extrinsics extrinsics_;
    SensorHealth health_;
    bool active_{false};

    uint64_t seq_{0};
    double lat_{37.7749}; // Default San Francisco coordinates
    double lon_{-122.4194};
    double alt_{10.0};
};

} // namespace uados::sensors
