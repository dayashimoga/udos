# UADOS — Master Decisions Log (ADRs)

> **Version**: 0.1.0  
> **Status**: Draft  
> **Last Updated**: 2026-05-30  
> **Owner**: UADOS Architecture Team

---

## ADR Index

| ID | Title | Status | Date |
|----|-------|--------|------|
| ADR-001 | [Microkernel Architecture](#adr-001-microkernel-architecture) | Accepted | 2026-05-30 |
| ADR-002 | [C++20 as Primary Runtime Language](#adr-002-c20-as-primary-runtime-language) | Accepted | 2026-05-30 |
| ADR-003 | [Zero-Copy Shared Memory Event Bus](#adr-003-zero-copy-shared-memory-event-bus) | Accepted | 2026-05-30 |
| ADR-004 | [FlatBuffers for Hot Path Serialization](#adr-004-flatbuffers-for-hot-path-serialization) | Accepted | 2026-05-30 |
| ADR-005 | [Plugin-Based Extension Model](#adr-005-plugin-based-extension-model) | Accepted | 2026-05-30 |
| ADR-006 | [Simulation-First Development](#adr-006-simulation-first-development) | Accepted | 2026-05-30 |
| ADR-007 | [Independent Safety Monitor](#adr-007-independent-safety-monitor) | Accepted | 2026-05-30 |
| ADR-008 | [Lanelet2 for HD Maps](#adr-008-lanelet2-for-hd-maps) | Accepted | 2026-05-30 |
| ADR-009 | [ONNX Runtime for ML Inference](#adr-009-onnx-runtime-for-ml-inference) | Accepted | 2026-05-30 |
| ADR-010 | [CMake + Conan 2 Build System](#adr-010-cmake--conan-2-build-system) | Accepted | 2026-05-30 |
| ADR-011 | [Pre-Production Safety Grade (ASIL-B)](#adr-011-pre-production-safety-grade-asil-b) | Accepted | 2026-05-30 |
| ADR-012 | [OpenTelemetry for Observability](#adr-012-opentelemetry-for-observability) | Accepted | 2026-05-30 |
| ADR-013 | [Rate-Monotonic Scheduling](#adr-013-rate-monotonic-scheduling) | Accepted | 2026-05-30 |
| ADR-014 | [Apache 2.0 License](#adr-014-apache-20-license) | Accepted | 2026-05-30 |
| ADR-015 | [CARLA as Primary Simulation Platform](#adr-015-carla-as-primary-simulation-platform) | Accepted | 2026-05-30 |

---

## ADR-001: Microkernel Architecture

**Status**: Accepted  
**Date**: 2026-05-30

### Context
UADOS needs an architecture that supports multiple vehicle platforms, allows independent subsystem development, and can evolve over multiple years without major rewrites.

### Decision
Adopt a microkernel architecture where the kernel provides only: lifecycle management, event bus, scheduling, health monitoring, and plugin loading. All domain logic (perception, planning, control) runs as plugins.

### Alternatives Considered
1. **Monolithic kernel** — Simpler initially but becomes unmaintainable; tight coupling makes testing and replacement difficult.
2. **ROS 2-based** — Mature ecosystem but brings heavy dependency, DDS overhead, and less control over scheduling/memory.
3. **Microservice architecture** — Over-engineered for single-vehicle deployment; inter-process overhead unacceptable for real-time.

### Consequences
- (+) Strong isolation between subsystems
- (+) Independent development and testing of each subsystem
- (+) Hot-swap capability for plugins
- (+) Minimal trusted computing base for safety
- (-) More complex IPC than monolithic
- (-) Need to build core infrastructure from scratch

---

## ADR-002: C++20 as Primary Runtime Language

**Status**: Accepted  
**Date**: 2026-05-30

### Context
The runtime system requires deterministic performance, zero-overhead abstractions, and control over memory allocation patterns.

### Decision
Use C++20 for all runtime components. Use Python 3.12 for tooling, ML training, simulation scripting, and test automation.

### Alternatives Considered
1. **Rust** — Superior memory safety but smaller automotive ecosystem; harder to find automotive Rust engineers; FFI overhead with ML libraries.
2. **C** — Maximum control but lacks abstractions; significantly more code; error-prone manual memory management.
3. **Python-only** — Unacceptable latency for real-time pipeline.

### Consequences
- (+) Industry-standard for automotive software
- (+) Mature tooling (clang, sanitizers, profilers)
- (+) Interoperability with Eigen, OpenCV, PCL, ONNX Runtime
- (+) C++20 features (concepts, ranges, coroutines) improve code quality
- (-) Memory safety requires discipline (mitigated by sanitizers, static analysis)
- (-) Longer compile times than C

---

## ADR-003: Zero-Copy Shared Memory Event Bus

**Status**: Accepted  
**Date**: 2026-05-30

### Context
The autonomy pipeline processes high-bandwidth sensor data (cameras: ~180 MB/s, LiDAR: ~36 MB/s). Copying this data between components is prohibitive.

### Decision
Implement a custom zero-copy event bus using POSIX shared memory. Messages are written once into a shared memory pool and consumers receive pointers. Reference counting manages lifetime.

### Alternatives Considered
1. **DDS (eProsima Fast DDS)** — Industry standard for ROS 2 but adds latency (~10-50μs), memory copies, and significant code bloat. Retained as optional bridge for fleet communication.
2. **ZeroMQ** — Good for inter-process but copies data. Not zero-copy for large messages.
3. **gRPC** — Too high latency for real-time; serialization overhead. Used only for fleet/cloud communication.

### Consequences
- (+) Sub-microsecond message delivery
- (+) No memory copies for sensor data
- (+) Predictable latency (no allocations)
- (-) Platform-specific (POSIX shared memory)
- (-) More complex error handling (shared memory corruption)
- (-) Need careful lifecycle management

---

## ADR-004: FlatBuffers for Hot Path Serialization

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Messages on the event bus need a schema-defined format for type safety and versioning, but deserialization overhead is unacceptable on the hot path.

### Decision
Use FlatBuffers for all hot-path messages (sensor data, perception output, control commands). Use Protocol Buffers for cold-path communication (configuration, fleet API, diagnostics).

### Alternatives Considered
1. **Protocol Buffers** — Good schema support but requires deserialization (copies). Used for cold path.
2. **Cap'n Proto** — Zero-copy like FlatBuffers but smaller ecosystem.
3. **Raw structs** — Fastest but no versioning, no schema evolution.

### Consequences
- (+) Zero-copy access to serialized data
- (+) Schema evolution with backward compatibility
- (+) Code generation for C++ and Python
- (-) Slightly more complex API than Protobuf
- (-) Less human-readable than JSON/Protobuf text format

---

## ADR-005: Plugin-Based Extension Model

**Status**: Accepted  
**Date**: 2026-05-30

### Context
UADOS must support multiple vehicle platforms, sensor configurations, and perception algorithms without modifying core code.

### Decision
All major subsystems are implemented as dynamically-loaded plugins with versioned C++ interfaces. Plugins are loaded via `dlopen`, instantiated via factory functions, and managed by the Plugin System.

### Alternatives Considered
1. **Static linking** — Simpler but no hot-swap; requires full rebuild for any change.
2. **Script-based plugins (Lua/Python)** — Too slow for real-time components.
3. **Container-based isolation** — Over-isolated for single-vehicle deployment.

### Consequences
- (+) Hot-swap without restart
- (+) Independent compilation and testing
- (+) Third-party extensions without source access
- (-) ABI compatibility challenges
- (-) More complex debugging (dynamic symbols)
- (-) Need versioned interface stability guarantees

---

## ADR-006: Simulation-First Development

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Physical vehicle testing is expensive, slow, and dangerous for early development. Every component must be validated before deployment.

### Decision
All components must be fully testable in simulation. CARLA is the primary simulation platform. Physical vehicle testing is a validation step, not a development step.

### Consequences
- (+) Faster development iteration
- (+) Safe testing of edge cases
- (+) Reproducible test scenarios
- (+) CI/CD integration (automated simulation tests)
- (-) Simulation-to-real gap must be actively managed
- (-) CARLA requires GPU resources for CI

---

## ADR-007: Independent Safety Monitor

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Safety monitoring must be independent of the systems it monitors. A bug in perception or planning must not compromise safety.

### Decision
The Safety Monitor runs in a separate OS process, on a separate CPU core (when possible), with its own event bus connection. It has authority to override all actuator commands and trigger emergency responses.

### Consequences
- (+) Fault isolation from monitored systems
- (+) Independent failure mode
- (+) Can detect system-level failures (process crashes, deadlocks)
- (-) Additional IPC latency for safety checks
- (-) Must maintain its own simplified world model

---

## ADR-008: Lanelet2 for HD Maps

**Status**: Accepted  
**Date**: 2026-05-30

### Context
HD maps are essential for localization, planning, and regulatory compliance. Need an open, well-defined map format.

### Decision
Use Lanelet2 as the primary HD map format. Build a map abstraction layer to support future format additions.

### Alternatives Considered
1. **OpenDRIVE** — Good road geometry but weaker semantic representation.
2. **HERE/TomTom proprietary** — Commercial dependency, licensing cost.
3. **Custom format** — Unnecessary when open standards exist.

### Consequences
- (+) Open-source, actively maintained
- (+) Rich semantic information (traffic rules, regulatory elements)
- (+) Good integration with CARLA and SUMO
- (-) Learning curve for Lanelet2 API
- (-) May need extensions for specialized use cases

---

## ADR-009: ONNX Runtime for ML Inference

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Perception models (detection, segmentation, lane detection) need efficient inference on various hardware (CPU, GPU, NPU).

### Decision
Use ONNX Runtime as the inference engine. All models are trained in PyTorch and exported to ONNX format.

### Alternatives Considered
1. **TensorRT** — Fastest on NVIDIA but vendor-locked.
2. **PyTorch C++ (LibTorch)** — Direct but heavy dependency, less optimized.
3. **TFLite** — Good for edge but limited model support.

### Consequences
- (+) Hardware-agnostic (CPU, CUDA, TensorRT, OpenVINO backends)
- (+) Single inference API for all models
- (+) Well-defined model format (ONNX)
- (-) Slightly slower than native TensorRT on NVIDIA
- (-) ONNX export can have edge cases for complex models

---

## ADR-010: CMake + Conan 2 Build System

**Status**: Accepted  
**Date**: 2026-05-30

### Context
The project needs a build system that supports cross-compilation, multiple compilers, and reproducible builds.

### Decision
Use CMake 3.28+ as the build system with Conan 2 for C++ dependency management. Use pyproject.toml for Python packages.

### Alternatives Considered
1. **Bazel** — Superior caching and hermeticity but steeper learning curve; less automotive adoption.
2. **Meson** — Faster than CMake but smaller ecosystem.
3. **colcon (ROS 2)** — ROS-specific, ties us to ROS ecosystem.

### Consequences
- (+) Widest ecosystem support
- (+) Conan 2 handles complex dependency graphs
- (+) Most automotive engineers know CMake
- (-) CMake syntax can be verbose
- (-) Conan 2 has breaking changes from Conan 1

---

## ADR-011: Pre-Production Safety Grade (ASIL-B)

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Full ASIL-D compliance requires 5-10x more engineering effort including formal methods, certified toolchains, and third-party audits. The initial system is a research/development platform.

### Decision
Design with ASIL-B patterns (documented hazard analysis, safety monitor, fault detection) but do not pursue formal certification in initial phases. Architecture supports upgrade to ASIL-D.

### Consequences
- (+) Practical safety without certification overhead
- (+) Architecture ready for future ASIL-D upgrade
- (+) Documented hazard analysis provides safety evidence
- (-) Not deployable on public roads without further certification
- (-) Some safety patterns may need rework for formal compliance

---

## ADR-012: OpenTelemetry for Observability

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Need vendor-neutral observability covering metrics, traces, and logs.

### Decision
Use OpenTelemetry SDK for instrumentation. Export to Prometheus (metrics) and Grafana (dashboards). Structured logging via spdlog.

### Consequences
- (+) Vendor-neutral, open standard
- (+) Single API for metrics, traces, logs
- (+) Rich ecosystem of exporters
- (-) C++ SDK less mature than Java/Go
- (-) Additional dependency

---

## ADR-013: Rate-Monotonic Scheduling

**Status**: Accepted  
**Date**: 2026-05-30

### Context
The autonomy pipeline has strict timing requirements. Components must execute at predictable rates.

### Decision
Use Rate-Monotonic Scheduling (RMS) where priority is inversely proportional to period. Deadline monitoring reports violations to Health Monitor.

### Alternatives Considered
1. **Earliest Deadline First (EDF)** — Optimal utilization but harder to analyze; priority inversion more complex.
2. **Round-robin** — Simple but no priority support.
3. **FIFO** — No preemption; unacceptable for mixed-criticality.

### Consequences
- (+) Well-understood schedulability analysis
- (+) Predictable worst-case behavior
- (+) Priority-based preemption
- (-) Lower theoretical utilization than EDF (~69% for harmonic periods)
- (-) Priority inversion needs protocol (priority inheritance/ceiling)

---

## ADR-014: Apache 2.0 License

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Need a permissive license that allows commercial use while providing patent protection.

### Decision
Apache License 2.0 for all UADOS source code.

### Consequences
- (+) Permissive, allows commercial use
- (+) Patent grant protects users
- (+) Compatible with most dependencies
- (-) Less copyleft protection than GPL

---

## ADR-015: CARLA as Primary Simulation Platform

**Status**: Accepted  
**Date**: 2026-05-30

### Context
Need a realistic simulation environment for developing and testing the full autonomy stack.

### Decision
Use CARLA as the primary simulation platform. Build a bridge to abstract CARLA specifics behind our sensor/driver interfaces.

### Alternatives Considered
1. **LGSVL** — Discontinued.
2. **NVIDIA DRIVE Sim** — Excellent quality but proprietary, expensive.
3. **AirSim** — Less maintained; smaller community.
4. **Custom engine** — Too expensive to build from scratch initially.

### Consequences
- (+) Open-source, active community
- (+) Realistic rendering (Unreal Engine)
- (+) Rich sensor simulation (camera, LiDAR, radar, GPS, IMU)
- (+) Python and C++ API
- (+) OpenDRIVE/Lanelet2 map support
- (-) Heavy GPU requirements
- (-) Simulation fidelity limitations (tire model, aerodynamics)
- (-) Some stability issues in edge cases

---

*End of Master Decisions Log*
