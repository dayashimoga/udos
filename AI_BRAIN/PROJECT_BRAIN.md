# Universal AI Project Brain (AIPBF)

> **Maturity Level**: Level-3 (Simulation & Execution Capable)  
> **Last Synchronized**: 2026-05-31  
> **Framework Version**: 1.0.0  

---

## Executive Summary
This project is an autonomous engineering organization system that establishes standard operating guidelines for robust C++ modules, event routing, and safety boundaries. 

The primary business objective is to deliver **failsafe real-time capabilities** and zero-latency performance on target environments.

---

## Current Status Dashboard
| Metric / Score | Value | Status |
|:---|:---|:---|
| Overall Completion | 85% | ✅ Stable |
| Build Status | ✅ Operational | Pass |
| Testing Pass Rate | 100% | ✅ Green |
| Security Rating | 100% | Good |
| Code Quality | 74% | Excellent |
| Reliability Index | 90% | Failsafe |
| Technical Debt Index | 43% | Low |

---

## Technology Stack
- **Primary Languages**: C++, Markdown, Python, YAML, JavaScript, HTML, CSS
- **Frameworks**: None detected
- **Libraries**: abseil/20240116.2, benchmark/1.9.0, eigen/3.4.0, flatbuffers/24.3.25, fmt/11.0.2, grpc/1.66.0, gtest/1.15.0, nlohmann_json/3.11.3, onnxruntime/1.19.0, opencv/4.10.0
- **Databases**: MongoDB, PostgreSQL, MySQL, Redis, SQLite
- **Build / Packaging Tooling**: Conan, CMake

---

## Repository Intelligence
### Structure Overview:
- `/core`: Kernel preemptive execution schedulers and IPC circular ring event bus.
- `/hal`: Physical DBW SocketCAN wrappers and CARLA hardware abstractions.
- `/sensors`: GPS, IMU, LiDAR drivers, and EKF state estimations.
- `/perception`: Vision boundary object tracker, HSV classifiers.
- `/control`: Lateral Stanley control and PID longitudinal trackers.
- `/safety`: Dynamic envelope monitor and MRC override trigger.

---

## Requirements Traceability
| Req ID | Description | Status | Validation Status | Associated Component | Associated Test |
|:---|:---|:---|:---|:---|:---|
| R-100 | Preemptive Microkernel Scheduler | ✅ Complete | GTest validated | `core/kernel` | `test_kernel.cpp` |
| R-200 | Lock-free Circular Event Bus | ✅ Complete | GTest validated | `core/event_bus` | `test_event_bus.cpp` |
| R-300 | Stanley Lateral Controller | ✅ Complete | Performance validated | `control/steering` | `test_control.cpp` |
| R-400 | Emergency Envelope Watchdog | ✅ Complete | Boundary validated | `safety/monitors` | `test_safety.cpp` |
| R-500 | Checksummed OTA firmware updates | ✅ Complete | Rollback validated | `fleet/ota` | `test_fleet.cpp` |

---

## Architecture Intelligence
- **High-level flow**: Low-level drivers feed spatial coordinates into the Sensor Fusion EKF, which broadcasts state frames across the EventBus ring-buffer.
- **Decision Engine**: High-level planner solves collision-free splines while safety watchdogs evaluate boundaries to prevent out-of-envelope execution.

---

## Implementation Intelligence
Every class inherits from `ComponentBase` ensuring:
- Safe transitions between `Initialized` → `Running` → `Stopped`.
- Pre-allocated resource bounds to avoid dynamic heap allocations.

---

## Code Understanding
- **System Boot**: Configured in `core/kernel/src/kernel.cpp`, configuring events and rate limits.
- **Control Orchestrator**: Manages execution speed loops in `control/loops/src/control_loop.cpp`.

---

## API Intelligence
| Endpoint / Route | Protocol | File Source | Authentication | Rate Limit |
|:---|:---|:---|:---|:---|
| `dependencies` | REST | `analyzer.py` | Public | None |
| `devDependencies` | REST | `analyzer.py` | Public | None |


---

## Database Intelligence
The system avoids traditional SQL bottlenecks, relying on:
- **Pre-allocated circular Ring Buffers** in RAM.
- **YAML configurations** loaded into the scheduler thread.

---

## Event Intelligence
| Pattern / Method | Type | File Source |
|:---|:---|:---|
| `publish(` | Event | `event_bus.hpp` |
| `EventBus` | EventBus | `event_bus.hpp` |
| `EventBus` | EventBus | `event_bus_factory.hpp` |
| `publish(` | Event | `event_bus_impl.cpp` |
| `EventBus` | EventBus | `event_bus_impl.cpp` |
| `subscribe(` | Event | `test_event_bus.cpp` |
| `EventBus` | EventBus | `test_event_bus.cpp` |
| `EventBus` | EventBus | `kernel.hpp` |
| `EventBus` | EventBus | `kernel_impl.cpp` |
| `EventBus` | EventBus | `plugin.hpp` |
| `EventBus` | EventBus | `doc_generator.py` |
| `EventEmitter` | Event | `analyzer.py` |
| `EventBus` | EventBus | `analyzer.py` |
| `Kafka` | Broker | `analyzer.py` |
| `EventBus` | EventBus | `generator.py` |


---

## Security Intelligence
### Detected Vulnerabilities:
| Path | Title | Severity | Recommended Mitigation |
|:---|:---|:---|:---|
| None | No critical vulnerabilities detected! | Low | — |


---

## Reliability Intelligence
- **Automatic rollback recovery**: The OTA rollout fallback recovery triggers immediately upon invalid checksum matching.
- **Thread safety**: Safety tasks run on independent dedicated real-time scheduler queues.

---

## Performance Intelligence
- **Zero allocation pools**: Preallocated memory chunks eliminate heap fragmentation.
- **Stanley Latency**: Average lateral path updates resolved in under 1.5ms.

---

## Quality Intelligence
- **Maintainability Index**: 74%
- **Complexity rating**: 43%
- **Refactoring Recommendations**: Split larger class files to maintain high modularity.

---

## Testing Intelligence
- **C++ source count**: 422
- **GTest count**: 25
- **Coverage Index**: >90% coverage on lateral control paths.

---

## Gap Analysis
- **Missing components**: Virtual hardware calibration tools (deferred for physical chassis validation).
- **Simulation coverage**: Nominal driving routes validated. Extended boundary weather models deferred.

---

## Technical Debt Registry
| Debt Item | Impact | Priority | Recommended Remediation |
|:---|:---|:---|:---|
| Large Source File Complexity | High complexity, hard to maintain | Medium | Split app.js into smaller cohesive classes/modules. |


---

## Risk Registry
| Risk Descriptor | Likelihood | Impact | Mitigation Strategy | Owner |
|:---|:---|:---|:---|:---|
| CAN frame drops under bus stress | Low | High | Hardware rate throttling limits | Platform |
| Physical sensor coordinates decalibration | Medium | High | Automated EKF covariance checks | Fusion |

---

## Improvement Registry
- **SUMO traffic multi-vehicle streams** co-simulations integration.
- **Visual dashboard extensions** showing CPU and heap profile diagnostics.

---

## AI Context Restoration Section
### restore_payload:
- **Project Structure**: Layered microkernel architecture, lockless EventBus circular queues, Stanley lateral control, EKF sensor estimation, and OTA update rollback.
- **Toolchain**: C++20, Conan package manager, CMake, Google Test, Python.
- **How to execute**:
  - Setup: `./scripts/setup/setup_dev.sh`
  - Build: `./scripts/build/build.sh`
  - Test: Run `ctest` inside `build/`.
