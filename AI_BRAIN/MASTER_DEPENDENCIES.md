# Master Dependencies Document (AIPBF v4.0)

> **Generated**: 2026-06-03
> **External Dependencies**: 13
> **Internal Dependencies**: 0

---

## External Dependencies


> **Verification**: VERIFIED  
> **Evidence**: File: `conanfile.py`, Line: 38, Confidence: HIGH  


| # | Dependency | Source |
|:---|:---|:---|
| 1 | `abseil/20240116.2` | Package manifest |
| 2 | `benchmark/1.9.0` | Package manifest |
| 3 | `eigen/3.4.0` | Package manifest |
| 4 | `flatbuffers/24.3.25` | Package manifest |
| 5 | `fmt/11.0.2` | Package manifest |
| 6 | `grpc/1.66.0` | Package manifest |
| 7 | `gtest/1.15.0` | Package manifest |
| 8 | `nlohmann_json/3.11.3` | Package manifest |
| 9 | `onnxruntime/1.19.0` | Package manifest |
| 10 | `opencv/4.10.0` | Package manifest |
| 11 | `protobuf/5.27.0` | Package manifest |
| 12 | `spdlog/1.14.1` | Package manifest |
| 13 | `yaml-cpp/0.8.0` | Package manifest |

---

## Internal Module Dependencies

| # | Module | Source |
|:---|:---|:---|
| None | No internal dependencies detected | N/A |

---

## Dependency Ownership Matrix

Strict subsystem architecture coupling boundaries (VERIFIED):

| Subsystem Component | Direct Hard Dependencies | Coupling Logic / Restrictions |
|:---|:---|:---|
| **core (Kernel)** | `common`, `eventbus`, `scheduler`, `health`, `lifecycle`, `plugin` | Real-time task schedulers, IPC, and dynamic hot-reload lifecycles. Zero external dependencies. |
| **sensors (HAL)** | `common`, `eventbus`, `digital_twin` | Read-only hardware streams, publishes raw sensor envelopes. Failsafe isolation. |
| **localization** | `common`, `eventbus` | Publishes odometry and EKF pose calculations. Zero control coupling. |
| **perception** | `common`, `eventbus`, `sensors` | Consumes raw feeds, publishes tracked objects and lane markings. Zero control coupling. |
| **prediction** | `common`, `eventbus`, `perception` | Calculates actor trajectory bounds. Zero motion solver dependencies. |
| **planning** | `common`, `eventbus`, `localization`, `prediction` | Jerk-limited motion solvers. Consumes pose and predictions to output optimal trajectory plans. |
| **control** | `common`, `eventbus`, `steering`, `throttle` | Closed-loop PID & Stanley solvers. Consumes planned trajectories. |
| **safety** | `common`, `eventbus`, `localization` | Independent ASIL-D collision checker. Can preempt any planned control frame. |
