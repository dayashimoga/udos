# UADOS — Master Test Status

> **Version**: 0.1.0  
> **Status**: Active  
> **Last Updated**: 2026-05-31  
> **Owner**: UADOS Architecture Team

---

## Test Coverage Summary

| Phase | Component | Test File | Unit | Integration | Fault Inj. | Safety | Edge Cases |
|-------|-----------|-----------|:---:|:---:|:---:|:---:|:---:|
| 2 | Kernel Core | `test_kernel.cpp` | ✅ | ✅ | — | — | 🟡 |
| 2 | Event Bus | `test_event_bus.cpp` | ✅ | ✅ | — | — | 🟡 |
| 2 | Scheduler | `test_scheduler.cpp` | ✅ | — | — | — | 🟡 |
| 2 | Health Monitor | `test_health.cpp` | ✅ | — | — | — | 🟡 |
| 2 | Lifecycle Mgr | `test_lifecycle.cpp` | ✅ | ✅ | — | — | 🟡 |
| 2 | Memory Pool | `test_memory_pool.cpp` | ✅ | — | — | — | ✅ |
| 2 | SPSC Queue | `test_spsc_queue.cpp` | ✅ | — | — | — | ✅ |
| 2 | Types | `test_types.cpp` | ✅ | — | — | — | ✅ |
| 2 | Version | `test_version.cpp` | ✅ | — | — | — | — |
| 3 | Safety Envelope | `test_safety_envelope.cpp` | ✅ | — | — | ✅ | 🟡 |
| 3 | Driver Validation | `test_driver_validation.cpp` | ✅ | ✅ | — | — | 🟡 |
| 4 | Sensors API | `test_sensors.cpp` | ✅ | ✅ | — | — | 🟡 |
| 4 | Sensor Fusion EKF | `test_sensor_fusion.cpp` | ✅ | ✅ | — | — | ✅ |
| 5 | Perception Pipeline | `test_perception.cpp` | ✅ | ✅ | — | — | 🟡 |
| 6 | Localization | `test_localization.cpp` | ✅ | ✅ | — | — | 🟡 |
| 7 | Prediction | `test_prediction.cpp` | ✅ | ✅ | — | — | 🟡 |
| 8 | Planning | `test_planning.cpp` | ✅ | ✅ | — | — | 🟡 |
| 9 | Control | `test_control.cpp` | ✅ | ✅ | — | ✅ | ✅ |
| 10 | Safety Monitor + ERS | `test_safety.cpp` | ✅ | ✅ | ✅ | ✅ | ✅ |
| 11 | Digital Twin | `test_digital_twin.cpp` | ✅ | ✅ | — | — | 🟡 |
| 12 | Simulation | `test_simulation.cpp` | ✅ | ✅ | — | ✅ | 🟡 |
| 13 | Validation | `test_validation.cpp` | ✅ | ✅ | ✅ | ✅ | ✅ |
| 14 | Fleet & OTA | `test_fleet.cpp` | ✅ | ✅ | ✅ | — | ✅ |
| 15 | Hardening Profiler | `test_hardening.cpp` | ✅ | — | — | — | ✅ |

**Legend**: ✅ Covered | 🟡 Basic coverage (needs edge cases) | — Not applicable / not yet

---

## Quality Gate Status

| Phase | Build | Lint | Unit Tests | Integration | Security | Docs | Coverage | Gate |
|-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | N/A | N/A | N/A | N/A | N/A | ✅ | N/A | ✅ |
| 1 | ✅ | ✅ | N/A | N/A | ✅ | ✅ | N/A | ✅ |
| 2 | ✅ | ✅ | ✅ | ✅ | 🟡 | ✅ | 🟡 | ✅ |
| 3 | ✅ | ✅ | ✅ | ✅ | 🟡 | ✅ | 🟡 | ✅ |
| 4 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 5 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 6 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 7 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 8 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 9 | ✅ | ✅ | ✅ | ✅ | — | ✅ | ✅ | ✅ |
| 10 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| 11 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 12 | ✅ | ✅ | ✅ | ✅ | — | ✅ | 🟡 | ✅ |
| 13 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| 14 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| 15 | ✅ | ✅ | ✅ | — | — | ✅ | 🟡 | ✅ |

**Legend**: ✅ Pass | 🟡 Partial | 🔴 Fail | ⬜ Not Started

---

## Test Suites Inventory

| Target Executable | Source File | Test Count (approx.) | Subsystems Covered |
|-------------------|-----------|:---:|-----|
| `uados_kernel_tests` | `test_kernel.cpp` | 8 | ConfigManager, Kernel lifecycle |
| `uados_event_bus_tests` | `test_event_bus.cpp` | 6 | Pub/sub, topic routing, zero-copy |
| `uados_scheduler_tests` | `test_scheduler.cpp` | 5 | RMS priority, deadline monitoring |
| `uados_health_tests` | `test_health.cpp` | 4 | Watchdog timers, heartbeat |
| `uados_lifecycle_tests` | `test_lifecycle.cpp` | 5 | State machine, multi-component |
| `uados_memory_pool_tests` | `test_memory_pool.cpp` | 6 | Treiber stack, pool exhaustion |
| `uados_spsc_queue_tests` | `test_spsc_queue.cpp` | 5 | Push/pop, overflow, empty |
| `uados_types_tests` | `test_types.cpp` | 8 | GeoCoordinate, WGS84 distances |
| `test_uados_hal` | `test_safety_envelope.cpp`, `test_driver_validation.cpp` | 10 | Safety limits, BOS, compliance |
| `test_uados_sensors` | `test_sensors.cpp`, `test_sensor_fusion.cpp` | 12 | All 5 sensor types, EKF fusion |
| `test_uados_perception` | `test_perception.cpp` | 8 | Detection, tracking, lanes, lights |
| `test_uados_localization` | `test_localization.cpp` | 6 | Pose, SLAM, HD Map queries |
| `test_uados_prediction` | `test_prediction.cpp` | 6 | CA trajectories, behavior, TTC risk |
| `test_uados_planning` | `test_planning.cpp` | 8 | Dijkstra, FSM, quintic splines |
| `test_uados_control` | `test_control.cpp` | 6 | Stanley, PID, safety gates |
| `test_uados_safety` | `test_safety.cpp` | 8 | Invariants, ERS FSM, ODD bounds |
| `test_uados_digital_twin` | `test_digital_twin.cpp` | 6 | Bicycle dynamics, sensor projection |
| `test_uados_simulation` | `test_simulation.cpp` | 6 | Scenario batch, replay serialization |
| `test_uados_validation` | `test_validation.cpp` | 8 | Automated tests, fault injection |
| `test_uados_fleet` | `test_fleet.cpp` | 8 | Telemetry, SemVer, DJB2, rollback |
| `uados_common_tests` | `test_hardening.cpp` | 4 | Profiler, jitter, heap leak audit |

**Total Estimated Tests**: ~150+

---

## Performance Benchmarks

| Component | Metric | Target | Status |
|-----------|--------|--------|:---:|
| Event Bus | Message latency (intra-process) | ≤ 1μs | 🟡 |
| Event Bus | Throughput | ≥ 1M msg/s | 🟡 |
| Scheduler | Determinism (jitter) | ≤ 100μs | 🟡 |
| Scheduler | Loop period gate | ≤ 2.0ms jitter on 10ms period | ✅ |
| Perception | Inference latency | ≤ 50ms | 🟡 |
| Planning | Cycle time | ≤ 20ms | 🟡 |
| Control | Loop frequency | ≥ 100Hz | ✅ |
| E2E Pipeline | Sensor-to-actuator latency | ≤ 100ms | 🟡 |
| Resource Profiler | Heap leak detection | 0 allocations on hot path | ✅ |

> **Note**: 🟡 benchmarks have been designed in code (ResourceProfiler) but not yet measured on target hardware (requires Linux with RT kernel or Jetson Orin).

---

*End of Master Test Status*
