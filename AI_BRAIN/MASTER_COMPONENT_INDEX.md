# UADOS — Master Component Index

> **Version**: 0.1.0  
> **Status**: Active  
> **Last Updated**: 2026-05-31  
> **Owner**: UADOS Architecture Team

---

## Component Registry

| ID | Component | Path | Phase | Status | Language | Test File |
|----|-----------|------|-------|--------|----------|-----------|
| C-001 | Build System | `CMakeLists.txt` | 1 | ✅ | CMake | — |
| C-002 | Dependency Manager | `conanfile.py` | 1 | ✅ | Python | — |
| C-003 | CI/CD Pipeline | `.github/workflows/ci.yml` | 1 | ✅ | YAML | — |
| C-004 | Documentation Platform | `docs/` | 1 | 🟡 | Markdown | — |
| C-005 | Dev Setup Script | `scripts/setup/` | 1 | ⬜ | Bash/PS1 | — |
| C-010 | Kernel Core | `core/kernel/` | 2 | ✅ | C++20 | `test_kernel.cpp` |
| C-011 | Event Bus | `core/event_bus/` | 2 | ✅ | C++20 | `test_event_bus.cpp` |
| C-012 | Scheduler | `core/scheduler/` | 2 | ✅ | C++20 | `test_scheduler.cpp` |
| C-013 | Health Monitor | `core/health/` | 2 | ✅ | C++20 | `test_health.cpp` |
| C-014 | Lifecycle Manager | `core/lifecycle/` | 2 | ✅ | C++20 | `test_lifecycle.cpp` |
| C-015 | Plugin System | `core/plugin/` | 2 | ✅ | C++20 | — |
| C-016 | Messaging Layer | `core/messaging/` | 2 | ✅ | C++20 | — |
| C-017 | Memory Pool | `core/kernel/` | 2 | ✅ | C++20 | `test_memory_pool.cpp` |
| C-018 | Configuration | `core/kernel/` | 2 | ✅ | C++20 | `test_kernel.cpp` |
| C-019 | Logging | `core/common/` | 2 | ✅ | C++20 | — |
| C-020 | Types & Constants | `core/common/` | 2 | ✅ | C++20 | `test_types.cpp` |
| C-021 | Resource Profiler | `core/common/` | 15 | ✅ | C++20 | `test_hardening.cpp` |
| C-030 | Vehicle API | `hal/api/` | 3 | ✅ | C++20 | `test_safety_envelope.cpp` |
| C-031 | Driver SDK | `hal/sdk/` | 3 | ✅ | C++20 | — |
| C-032 | CARLA Sim Driver | `hal/drivers/simulation/` | 3 | ✅ | C++20 | — |
| C-033 | RC Car Driver | `hal/drivers/rc_car/` | 3 | ✅ | C++20 | — |
| C-034 | CAN Bus Driver | `hal/drivers/canbus/` | 3 | ✅ | C++20 | — |
| C-035 | Driver Validation | `hal/validation/` | 3 | ✅ | C++20 | `test_driver_validation.cpp` |
| C-040 | Sensor API | `sensors/api/` | 4 | ✅ | C++20 | `test_sensors.cpp` |
| C-041 | Camera Driver | `sensors/camera/` | 4 | ✅ | C++20 | `test_sensors.cpp` |
| C-042 | Radar Driver | `sensors/radar/` | 4 | ✅ | C++20 | `test_sensors.cpp` |
| C-043 | LiDAR Driver | `sensors/lidar/` | 4 | ✅ | C++20 | `test_sensors.cpp` |
| C-044 | GPS Driver | `sensors/gps/` | 4 | ✅ | C++20 | `test_sensors.cpp` |
| C-045 | IMU Driver | `sensors/imu/` | 4 | ✅ | C++20 | `test_sensors.cpp` |
| C-046 | Sensor Fusion (EKF) | `sensors/fusion/` | 4 | ✅ | C++20 | `test_sensor_fusion.cpp` |
| C-047 | Calibration | `sensors/calibration/` | 4 | ⬜ | — | — |
| C-050 | Object Detection | `perception/detection/` | 5 | ✅ | C++20 | `test_perception.cpp` |
| C-051 | Classification | `perception/classification/` | 5 | ⬜ | — | — |
| C-052 | Multi-Object Tracking | `perception/tracking/` | 5 | ✅ | C++20 | `test_perception.cpp` |
| C-053 | Segmentation | `perception/segmentation/` | 5 | ⬜ | — | — |
| C-054 | Lane Detection | `perception/lanes/` | 5 | ✅ | C++20 | `test_perception.cpp` |
| C-055 | Sign Recognition | `perception/signs/` | 5 | ⬜ | — | — |
| C-056 | Traffic Light Detector | `perception/traffic_lights/` | 5 | ✅ | C++20 | `test_perception.cpp` |
| C-060 | GPS Fusion | `localization/gps_fusion/` | 6 | ✅ | C++20 | `test_localization.cpp` |
| C-061 | Visual Localization | `localization/visual/` | 6 | ✅ | C++20 | — |
| C-062 | SLAM Engine | `localization/slam/` | 6 | ✅ | C++20 | `test_localization.cpp` |
| C-063 | HD Map Engine | `localization/hdmap/` | 6 | ✅ | C++20 | `test_localization.cpp` |
| C-064 | Pose Estimator | `localization/pose/` | 6 | ✅ | C++20 | `test_localization.cpp` |
| C-070 | Trajectory Prediction | `prediction/trajectory/` | 7 | ✅ | C++20 | `test_prediction.cpp` |
| C-071 | Behavior Prediction | `prediction/behavior/` | 7 | ✅ | C++20 | `test_prediction.cpp` |
| C-072 | Risk Estimation | `prediction/risk/` | 7 | ✅ | C++20 | `test_prediction.cpp` |
| C-080 | Strategic Planner | `planning/strategic/` | 8 | ✅ | C++20 | `test_planning.cpp` |
| C-081 | Behavior Planner | `planning/behavior/` | 8 | ✅ | C++20 | `test_planning.cpp` |
| C-082 | Motion Planner | `planning/motion/` | 8 | ✅ | C++20 | `test_planning.cpp` |
| C-090 | Stanley Controller | `control/steering/` | 9 | ✅ | C++20 | `test_control.cpp` |
| C-091 | Brake Controller | `control/brake/` | 9 | ✅ | C++20 | — |
| C-092 | Longitudinal Controller | `control/throttle/` | 9 | ✅ | C++20 | `test_control.cpp` |
| C-093 | Control Loop Orchestrator | `control/loops/` | 9 | ✅ | C++20 | `test_control.cpp` |
| C-094 | Transmission Controller | `control/transmission/` | 9 | ✅ | C++20 | — |
| C-100 | Safety Monitor | `safety/monitors/` | 10 | ✅ | C++20 | `test_safety.cpp` |
| C-101 | Fault Detection | `safety/fault_detection/` | 10 | ✅ | C++20 | — |
| C-102 | Runtime Validation | `safety/runtime_validation/` | 10 | ✅ | C++20 | — |
| C-103 | Emergency Response System | `safety/emergency/` | 10 | ✅ | C++20 | `test_safety.cpp` |
| C-110 | Vehicle Twin | `digital_twin/vehicle/` | 11 | ✅ | C++20 | `test_digital_twin.cpp` |
| C-111 | Sensor Twin | `digital_twin/sensor/` | 11 | ✅ | C++20 | `test_digital_twin.cpp` |
| C-112 | Road Twin | `digital_twin/road/` | 11 | ⬜ | — | — |
| C-113 | Traffic Twin | `digital_twin/traffic/` | 11 | ⬜ | — | — |
| C-114 | Weather Twin | `digital_twin/weather/` | 11 | ⬜ | — | — |
| C-120 | Scenario Engine | `simulation/scenarios/` | 12 | ✅ | C++20 | `test_simulation.cpp` |
| C-121 | Sim Orchestrator | `simulation/orchestration/` | 12 | ⬜ | — | — |
| C-122 | Replay System | `simulation/replay/` | 12 | ✅ | C++20 | `test_simulation.cpp` |
| C-123 | Metrics Collector | `simulation/metrics/` | 12 | ⬜ | — | — |
| C-130 | Automated Validator | `validation/automated/` | 13 | ✅ | C++20 | `test_validation.cpp` |
| C-131 | Regression Framework | `validation/regression/` | 13 | ⬜ | — | — |
| C-132 | Performance Framework | `validation/performance/` | 13 | ⬜ | — | — |
| C-133 | Chaos Testing | `validation/chaos/` | 13 | ⬜ | — | — |
| C-134 | Fault Injector | `validation/fault_injection/` | 13 | ✅ | C++20 | `test_validation.cpp` |
| C-140 | Fleet Telemetry | `fleet/telemetry/` | 14 | ✅ | C++20 | `test_fleet.cpp` |
| C-141 | OTA Manager | `fleet/ota/` | 14 | ✅ | C++20 | `test_fleet.cpp` |
| C-142 | Remote Diagnostics | `fleet/diagnostics/` | 14 | ⬜ | — | — |
| C-143 | Fleet Analytics | `fleet/analytics/` | 14 | ⬜ | — | — |
| C-150 | UADOS CLI | `tools/cli/` | 1 | ⬜ | — | — |
| C-151 | Web Dashboard | `tools/dashboard/` | 12 | ✅ | HTML/JS/CSS | — |
| C-152 | Code Generators | `tools/codegen/` | 1 | ⬜ | — | — |

---

## Component Statistics

| Metric | Count |
|--------|-------|
| Total Declared Components | 78 |
| ✅ Implemented & Tested | 48 |
| ✅ Implemented (no dedicated test) | 10 |
| ⬜ Not Yet Implemented (scaffold only) | 17 |
| 🟡 Partial | 3 |
| Phases Covered | 1–15 |

---

## Unimplemented Components (Future Roadmap)

The following components exist only as directory scaffolds (`.gitkeep`) and are deferred to future development cycles:

| ID | Component | Rationale |
|----|-----------|-----------|
| C-005 | Dev Setup Script | Low priority; developers use manual Conan/CMake |
| C-047 | Calibration Tool | Requires physical sensor hardware |
| C-051 | Classification | Object detection covers basic classification |
| C-053 | Segmentation | Requires large model + GPU inference |
| C-055 | Sign Recognition | Covered partially by traffic light detector |
| C-112 | Road Twin | Requires full Lanelet2 map integration |
| C-113 | Traffic Twin | Requires SUMO co-simulation |
| C-114 | Weather Twin | Low priority for initial deployment |
| C-121 | Sim Orchestrator | Covered by ScenarioEngine for batch runs |
| C-123 | Metrics Collector | Prometheus integration deferred |
| C-131 | Regression Framework | Basic regression covered by GTest suites |
| C-132 | Performance Framework | Covered by ResourceProfiler for now |
| C-133 | Chaos Testing | Covered by FaultInjector component |
| C-142 | Remote Diagnostics | gRPC integration deferred |
| C-143 | Fleet Analytics | Grafana integration deferred |
| C-150 | UADOS CLI | Low priority tooling |
| C-152 | Code Generators | Low priority tooling |

---

**Legend**: ⬜ Not Started | 🟡 In Progress | ✅ Complete | 🔴 Blocked

---

*End of Master Component Index*
