# UADOS — Master Requirements Document

> **Version**: 0.1.0  
> **Status**: Draft  
> **Last Updated**: 2026-05-30  
> **Owner**: UADOS Architecture Team

---

## Table of Contents

1. [System Vision](#1-system-vision)
2. [Stakeholders](#2-stakeholders)
3. [System-Wide Non-Functional Requirements](#3-system-wide-non-functional-requirements)
4. [Phase Requirements](#4-phase-requirements)
5. [Success Metrics](#5-success-metrics)
6. [Acceptance Criteria](#6-acceptance-criteria)
7. [Traceability Matrix](#7-traceability-matrix)

---

## 1. System Vision

UADOS is a **Universal Autonomous Driving Operating System** — a modular, safety-critical, platform-agnostic autonomy stack designed to:

- Operate on **multiple vehicle platforms** through a driver abstraction architecture
- Scale from **simulation** to **physical vehicles** without architectural changes
- Support the full **perception → planning → control** pipeline
- Provide **fleet-scale management** and **over-the-air updates**
- Maintain **safety** as an independent, non-overridable system guarantee
- Enable **continuous validation** through digital twins and simulation
- Be **extensible** through a plugin architecture without core modifications

---

## 2. Stakeholders

| Stakeholder | Role | Concerns |
|-------------|------|----------|
| Product Owner | Strategic direction | Feature priority, roadmap, business value |
| Safety Engineer | Safety assurance | ASIL compliance, hazard analysis, fault tolerance |
| Vehicle OEM | Integration partner | Driver compatibility, CAN/LIN integration, certification |
| Regulatory Body | Compliance | ISO 26262, UN R157, SOTIF (ISO 21448) |
| Fleet Operator | Operations | Telemetry, OTA, diagnostics, uptime |
| ML/AI Engineer | Perception & prediction | Model accuracy, inference latency, training pipeline |
| Simulation Engineer | Validation | Scenario coverage, replay fidelity, digital twin accuracy |

---

## 3. System-Wide Non-Functional Requirements

### 3.1 Performance

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-PERF-001 | End-to-end pipeline latency (sensor → actuator) | ≤ 100ms @ 10Hz | Critical |
| NFR-PERF-002 | Perception inference latency | ≤ 50ms per frame | Critical |
| NFR-PERF-003 | Planning cycle time | ≤ 20ms | Critical |
| NFR-PERF-004 | Control loop frequency | ≥ 100Hz | Critical |
| NFR-PERF-005 | Event bus message latency (intra-process) | ≤ 1μs (zero-copy) | High |
| NFR-PERF-006 | Event bus message latency (inter-process) | ≤ 100μs | High |
| NFR-PERF-007 | Sensor fusion cycle time | ≤ 10ms | Critical |
| NFR-PERF-008 | System boot to operational | ≤ 5 seconds | Medium |
| NFR-PERF-009 | Hot-swap plugin load time | ≤ 500ms | Medium |
| NFR-PERF-010 | Memory allocation on hot path | Zero (pre-allocated pools) | Critical |

### 3.2 Reliability

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-REL-001 | System uptime (per driving session) | ≥ 99.999% | Critical |
| NFR-REL-002 | Mean time between critical failures | ≥ 10,000 hours | Critical |
| NFR-REL-003 | Graceful degradation on component failure | Required | Critical |
| NFR-REL-004 | Automatic failover time | ≤ 50ms | Critical |
| NFR-REL-005 | Data pipeline durability | Zero message loss on hot path | Critical |
| NFR-REL-006 | Watchdog timeout detection | ≤ 100ms | Critical |

### 3.3 Safety

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-SAF-001 | Safety monitor independence | Separate process, separate core | Critical |
| NFR-SAF-002 | Emergency stop latency | ≤ 50ms from detection | Critical |
| NFR-SAF-003 | Fault detection coverage | ≥ 95% of identified failure modes | Critical |
| NFR-SAF-004 | Safety envelope enforcement | Always active, non-overridable | Critical |
| NFR-SAF-005 | Minimum risk condition (MRC) reachability | From any operational state | Critical |
| NFR-SAF-006 | Hazard analysis completeness | All HARA items addressed | Critical |
| NFR-SAF-007 | Runtime assertion failure handling | Safe stop, never crash-to-undefined | Critical |
| NFR-SAF-008 | Dual-channel safety validation | Independent checker for all safety-critical outputs | High |

### 3.4 Scalability

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-SCA-001 | Concurrent sensor streams | ≥ 16 sensors | High |
| NFR-SCA-002 | Fleet management scale | 1–1,000 vehicles | Medium |
| NFR-SCA-003 | Simulation parallelism | ≥ 100 concurrent scenarios | Medium |
| NFR-SCA-004 | Plugin count without performance degradation | ≥ 50 plugins | Medium |
| NFR-SCA-005 | HD map coverage area | City-scale (≥ 1,000 km²) | Medium |

### 3.5 Maintainability

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-MNT-001 | Code documentation coverage | 100% public API | High |
| NFR-MNT-002 | Test coverage (line) | ≥ 90% per component | High |
| NFR-MNT-003 | Cyclomatic complexity per function | ≤ 15 | Medium |
| NFR-MNT-004 | Module coupling | Loose (interface-only dependencies) | High |
| NFR-MNT-005 | Build time (incremental) | ≤ 30 seconds | Medium |
| NFR-MNT-006 | Build time (clean) | ≤ 10 minutes | Medium |

### 3.6 Security

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-SEC-001 | Inter-process authentication | Mutual TLS or equivalent | High |
| NFR-SEC-002 | OTA update integrity | Code-signed, verified chain | Critical |
| NFR-SEC-003 | CAN bus message authentication | MAC-based where supported | High |
| NFR-SEC-004 | Secrets management | No hardcoded secrets; vault-based | High |
| NFR-SEC-005 | Attack surface minimization | Minimal exposed interfaces | High |
| NFR-SEC-006 | Intrusion detection | Runtime anomaly detection on vehicle bus | Medium |

### 3.7 Observability

| ID | Requirement | Target | Priority |
|----|------------|--------|----------|
| NFR-OBS-001 | Structured logging | All components, machine-parseable | High |
| NFR-OBS-002 | Metrics emission | OpenTelemetry-compatible | High |
| NFR-OBS-003 | Distributed tracing | End-to-end pipeline tracing | High |
| NFR-OBS-004 | Real-time dashboard latency | ≤ 1 second | Medium |
| NFR-OBS-005 | Data recording for replay | All sensor data + internal state | High |
| NFR-OBS-006 | Alert routing | Configurable severity-based routing | Medium |

---

## 4. Phase Requirements

### Phase 1 — Foundation Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-FND-001 | CMake-based build system with cross-compilation support | Functional | Critical |
| FR-FND-002 | Conan 2 dependency management with lockfile support | Functional | Critical |
| FR-FND-003 | C++20 and Python 3.12 project scaffolding | Functional | Critical |
| FR-FND-004 | GitHub Actions CI pipeline (build, lint, test, coverage) | Functional | High |
| FR-FND-005 | Doxygen + Sphinx documentation generation | Functional | High |
| FR-FND-006 | clang-format and clang-tidy configuration | Functional | High |
| FR-FND-007 | Python linting (ruff) and formatting (black) configuration | Functional | High |
| FR-FND-008 | OpenTelemetry integration skeleton | Functional | High |
| FR-FND-009 | Development environment setup script | Functional | Medium |
| FR-FND-010 | Git hooks for pre-commit validation | Functional | Medium |

### Phase 2 — Vehicle OS Kernel

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-KRN-001 | Microkernel with minimal trusted computing base | Functional | Critical |
| FR-KRN-002 | Zero-copy shared-memory event bus | Functional | Critical |
| FR-KRN-003 | Deterministic priority-based task scheduler | Functional | Critical |
| FR-KRN-004 | Component lifecycle management (init → running → paused → stopped → error) | Functional | Critical |
| FR-KRN-005 | Health monitoring with configurable watchdog timeouts | Functional | Critical |
| FR-KRN-006 | Plugin system with versioned interfaces and hot-reload | Functional | High |
| FR-KRN-007 | Structured logging framework | Functional | High |
| FR-KRN-008 | Configuration management (YAML/TOML based) | Functional | High |
| FR-KRN-009 | Inter-process communication (Unix domain sockets + shared memory) | Functional | High |
| FR-KRN-010 | Time synchronization service (PTP/NTP aware) | Functional | High |
| FR-KRN-011 | Memory pool allocator for real-time components | Functional | High |
| FR-KRN-012 | Signal handling and graceful shutdown | Functional | High |

### Phase 3 — Vehicle Abstraction Layer

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-VAL-001 | Unified Vehicle API abstracting all actuators and sensors | Functional | Critical |
| FR-VAL-002 | Driver SDK with C++ and Python bindings | Functional | Critical |
| FR-VAL-003 | Driver interface: `init()`, `start()`, `stop()`, `read()`, `write()`, `status()` | Functional | Critical |
| FR-VAL-004 | CARLA simulation driver (reference implementation) | Functional | Critical |
| FR-VAL-005 | CAN bus generic driver framework | Functional | High |
| FR-VAL-006 | Driver validation framework (compliance test suite) | Functional | High |
| FR-VAL-007 | Vehicle state model (position, velocity, acceleration, orientation) | Functional | Critical |
| FR-VAL-008 | Actuator command interface (steering angle, brake pressure, throttle position) | Functional | Critical |
| FR-VAL-009 | Driver hot-swap without system restart | Functional | Medium |
| FR-VAL-010 | Vehicle capability discovery and negotiation | Functional | Medium |

### Phase 4 — Sensor Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-SEN-001 | Unified sensor interface for all sensor types | Functional | Critical |
| FR-SEN-002 | Camera driver framework (USB, MIPI CSI, GigE Vision) | Functional | High |
| FR-SEN-003 | Radar driver framework (CAN-based, Ethernet-based) | Functional | High |
| FR-SEN-004 | LiDAR driver framework (Velodyne, Ouster, Hesai protocols) | Functional | High |
| FR-SEN-005 | GPS/GNSS driver framework (NMEA, UBX) | Functional | High |
| FR-SEN-006 | IMU driver framework (SPI, I2C, serial) | Functional | High |
| FR-SEN-007 | Sensor calibration storage and loading | Functional | High |
| FR-SEN-008 | Sensor synchronization (hardware trigger + software sync) | Functional | Critical |
| FR-SEN-009 | Sensor fusion foundation (EKF/UKF based) | Functional | Critical |
| FR-SEN-010 | Sensor health monitoring and degradation detection | Functional | High |
| FR-SEN-011 | Raw data recording for offline replay | Functional | High |

### Phase 5 — Perception

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-PER-001 | 2D object detection (vehicles, pedestrians, cyclists, etc.) | Functional | Critical |
| FR-PER-002 | 3D object detection (LiDAR + camera fusion) | Functional | Critical |
| FR-PER-003 | Object classification with confidence scores | Functional | Critical |
| FR-PER-004 | Multi-object tracking (MOT) with track management | Functional | Critical |
| FR-PER-005 | Semantic segmentation (road, sidewalk, vegetation, etc.) | Functional | High |
| FR-PER-006 | Lane detection and lane boundary estimation | Functional | Critical |
| FR-PER-007 | Traffic sign detection and classification | Functional | High |
| FR-PER-008 | Traffic light detection and state recognition | Functional | Critical |
| FR-PER-009 | Free space estimation | Functional | High |
| FR-PER-010 | Occupancy grid generation | Functional | High |
| FR-PER-011 | Perception output in standardized world-frame coordinates | Functional | Critical |
| FR-PER-012 | Model versioning and A/B testing support | Functional | Medium |

### Phase 6 — Localization

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-LOC-001 | GPS/GNSS fusion with INS (EKF-based) | Functional | Critical |
| FR-LOC-002 | Visual localization (feature matching against HD map) | Functional | High |
| FR-LOC-003 | LiDAR-based SLAM | Functional | High |
| FR-LOC-004 | HD map loading and querying (Lanelet2 format) | Functional | Critical |
| FR-LOC-005 | 6-DOF pose estimation | Functional | Critical |
| FR-LOC-006 | Localization confidence estimation | Functional | Critical |
| FR-LOC-007 | Multi-source localization fusion | Functional | High |
| FR-LOC-008 | Map-relative positioning (lane-level accuracy) | Functional | Critical |
| FR-LOC-009 | Localization degradation detection and fallback | Functional | Critical |

### Phase 7 — Prediction

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-PRD-001 | Multi-modal trajectory prediction (≥ 3 hypotheses per agent) | Functional | Critical |
| FR-PRD-002 | Behavior prediction (lane change, turn, stop, yield) | Functional | Critical |
| FR-PRD-003 | Risk estimation per predicted trajectory | Functional | Critical |
| FR-PRD-004 | Prediction horizon ≥ 5 seconds | Functional | Critical |
| FR-PRD-005 | Interaction-aware prediction (agent-to-agent) | Functional | High |
| FR-PRD-006 | Prediction confidence and uncertainty quantification | Functional | High |
| FR-PRD-007 | Pedestrian intent prediction | Functional | High |

### Phase 8 — Planning

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-PLN-001 | Strategic planner (route planning on road graph) | Functional | Critical |
| FR-PLN-002 | Behavior planner (lane selection, speed profile, maneuver selection) | Functional | Critical |
| FR-PLN-003 | Motion planner (trajectory generation with kinematic constraints) | Functional | Critical |
| FR-PLN-004 | Collision avoidance constraint enforcement | Functional | Critical |
| FR-PLN-005 | Traffic rule compliance (speed limits, right-of-way, signals) | Functional | Critical |
| FR-PLN-006 | Comfort constraints (jerk limits, lateral acceleration limits) | Functional | High |
| FR-PLN-007 | Re-planning capability at ≥ 10Hz | Functional | Critical |
| FR-PLN-008 | Fallback trajectory generation (always available safe trajectory) | Functional | Critical |
| FR-PLN-009 | Multi-objective cost function (safety, comfort, efficiency, compliance) | Functional | High |

### Phase 9 — Control

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-CTL-001 | Lateral control (steering) with PID + feedforward | Functional | Critical |
| FR-CTL-002 | Longitudinal control (brake + throttle) | Functional | Critical |
| FR-CTL-003 | Model Predictive Control (MPC) option | Functional | High |
| FR-CTL-004 | Control loop frequency ≥ 100Hz | Functional | Critical |
| FR-CTL-005 | Actuator saturation handling | Functional | Critical |
| FR-CTL-006 | Trajectory tracking error monitoring | Functional | Critical |
| FR-CTL-007 | Smooth handover between control modes | Functional | High |
| FR-CTL-008 | Emergency braking override | Functional | Critical |
| FR-CTL-009 | Gear/transmission control interface | Functional | Medium |

### Phase 10 — Safety Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-SFT-001 | Independent safety monitor process | Functional | Critical |
| FR-SFT-002 | Runtime invariant checking (speed, acceleration, proximity) | Functional | Critical |
| FR-SFT-003 | Fault detection and isolation (FDI) | Functional | Critical |
| FR-SFT-004 | Emergency response system (safe stop, MRC) | Functional | Critical |
| FR-SFT-005 | Safety envelope computation and enforcement | Functional | Critical |
| FR-SFT-006 | Redundant perception cross-check | Functional | High |
| FR-SFT-007 | Actuator command plausibility check | Functional | Critical |
| FR-SFT-008 | Operational Design Domain (ODD) monitoring | Functional | Critical |
| FR-SFT-009 | Safety event logging (tamper-proof) | Functional | High |
| FR-SFT-010 | Driver/operator alerting system | Functional | High |

### Phase 11 — Digital Twin Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-DTW-001 | Vehicle digital twin (dynamics, kinematics, actuator models) | Functional | High |
| FR-DTW-002 | Sensor digital twin (noise models, FOV, occlusion) | Functional | High |
| FR-DTW-003 | Road network digital twin (from HD map) | Functional | High |
| FR-DTW-004 | Traffic agent digital twin (vehicle, pedestrian, cyclist behavior) | Functional | High |
| FR-DTW-005 | Weather/lighting digital twin (rain, fog, sun glare, night) | Functional | Medium |
| FR-DTW-006 | Twin synchronization with physical vehicle (when connected) | Functional | Medium |
| FR-DTW-007 | Twin state serialization for replay | Functional | High |

### Phase 12 — Simulation Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-SIM-001 | Scenario definition language (OpenSCENARIO 2.0 compatible) | Functional | High |
| FR-SIM-002 | Scenario generation (parametric, adversarial, corner-case) | Functional | High |
| FR-SIM-003 | Simulation orchestration (batch, parallel, CI-integrated) | Functional | High |
| FR-SIM-004 | CARLA bridge integration | Functional | High |
| FR-SIM-005 | SUMO traffic simulation bridge | Functional | Medium |
| FR-SIM-006 | Replay system (sensor + state playback) | Functional | High |
| FR-SIM-007 | Metrics collection and aggregation | Functional | High |
| FR-SIM-008 | Simulation-to-real gap analysis tools | Functional | Medium |

### Phase 13 — Validation Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-VLD-001 | Automated test execution and reporting | Functional | High |
| FR-VLD-002 | Regression test framework | Functional | High |
| FR-VLD-003 | Performance benchmarking framework | Functional | High |
| FR-VLD-004 | Chaos testing (random fault injection) | Functional | High |
| FR-VLD-005 | Targeted fault injection (specific failure modes) | Functional | High |
| FR-VLD-006 | Coverage analysis (code, requirement, scenario) | Functional | High |
| FR-VLD-007 | Validation evidence generation (reports, charts, logs) | Functional | High |

### Phase 14 — Fleet Platform

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-FLT-001 | Real-time fleet telemetry ingestion | Functional | High |
| FR-FLT-002 | OTA update management (staged rollout, rollback) | Functional | Critical |
| FR-FLT-003 | Remote diagnostics and log retrieval | Functional | High |
| FR-FLT-004 | Fleet analytics dashboard | Functional | Medium |
| FR-FLT-005 | Vehicle health scoring | Functional | Medium |
| FR-FLT-006 | Geofence management | Functional | Medium |

### Phase 15 — Production Hardening

| ID | Requirement | Type | Priority |
|----|------------|------|----------|
| FR-PRH-001 | Performance profiling and optimization pass | Functional | High |
| FR-PRH-002 | Security audit and penetration testing | Functional | Critical |
| FR-PRH-003 | Memory leak detection and elimination | Functional | High |
| FR-PRH-004 | Stress testing under sustained load | Functional | High |
| FR-PRH-005 | Operational runbook generation | Functional | High |
| FR-PRH-006 | Disaster recovery procedures | Functional | High |

---

## 5. Success Metrics

### Per-Phase Quality Gates

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Build Success Rate | 100% | CI pipeline |
| Lint Pass Rate | 100% | clang-tidy + ruff |
| Unit Test Pass Rate | 100% | CTest + pytest |
| Integration Test Pass Rate | 100% | Custom integration harness |
| System Test Pass Rate | 100% | System-level test orchestrator |
| Code Coverage (line) | ≥ 90% | gcov + coverage.py |
| Code Coverage (branch) | ≥ 80% | gcov + coverage.py |
| Documentation Coverage | 100% public API | Doxygen + Sphinx coverage report |
| Security Scan | 0 critical/high findings | CodeQL + Snyk + custom checks |
| Static Analysis | 0 warnings (with approved suppressions) | clang-tidy + cppcheck |

### System-Level Success Metrics

| Metric | Target | Phase |
|--------|--------|-------|
| End-to-end latency (sim) | ≤ 100ms | Phase 9+ |
| Perception mAP (KITTI benchmark) | ≥ 70% | Phase 5 |
| Localization accuracy (RMSE) | ≤ 10cm (with HD map) | Phase 6 |
| Prediction ADE (5s horizon) | ≤ 2.0m | Phase 7 |
| Planning success rate (sim scenarios) | ≥ 95% | Phase 8 |
| Control tracking error (RMSE) | ≤ 5cm lateral, ≤ 0.5 m/s longitudinal | Phase 9 |
| Safety monitor detection rate | ≥ 99% of injected faults | Phase 10 |
| Simulation throughput | ≥ 10x real-time | Phase 12 |
| Fleet OTA success rate | ≥ 99.9% | Phase 14 |
| System uptime (24h stress test) | ≥ 99.99% | Phase 15 |

---

## 6. Acceptance Criteria

### Phase 0 — Requirements & Architecture
- [ ] All master documents generated and internally consistent
- [ ] Architecture diagrams cover all major subsystems
- [ ] Risk registry identifies ≥ 20 risks with mitigations
- [ ] Traceability matrix links every requirement to a component
- [ ] Product Owner approval obtained

### Phase 1 — Foundation Platform
- [ ] Clean build on Ubuntu 22.04 and 24.04
- [ ] All CI pipeline stages pass (build, lint, test, docs)
- [ ] Development environment reproducible via setup script
- [ ] Documentation site generates and serves locally
- [ ] ≥ 90% test coverage on foundation code

### Phase 2 — Vehicle OS Kernel
- [ ] Event bus delivers 1M messages/second with < 1μs latency
- [ ] Scheduler demonstrates deterministic priority ordering
- [ ] Plugin system loads/unloads plugins without restart
- [ ] Health monitor detects and reports hung components within 100ms
- [ ] ≥ 90% test coverage

### Phase 3 — Vehicle Abstraction Layer
- [ ] CARLA simulation driver passes full compliance test suite
- [ ] Vehicle API supports read/write for all actuators and sensors
- [ ] Driver SDK compiles for C++ and Python
- [ ] Driver hot-swap demonstrated in simulation
- [ ] ≥ 90% test coverage

### Phase 4 — Sensor Platform
- [ ] Camera, Radar, LiDAR, GPS, IMU drivers operational in simulation
- [ ] Sensor fusion produces fused output at ≥ 10Hz
- [ ] Sensor calibration loads and applies correctly
- [ ] Data recording captures all sensor streams
- [ ] ≥ 90% test coverage

### Phase 5 — Perception
- [ ] Object detection achieves ≥ 70% mAP on benchmark dataset
- [ ] Tracking maintains ≥ 80% MOTA
- [ ] Lane detection operates at ≥ 10Hz
- [ ] Traffic light recognition ≥ 95% accuracy
- [ ] ≥ 90% test coverage

### Phase 6 — Localization
- [ ] GPS/INS fusion achieves ≤ 50cm RMSE (open sky)
- [ ] HD map-relative localization achieves ≤ 10cm RMSE
- [ ] SLAM operates in real-time (≥ 10Hz)
- [ ] Degradation detection triggers within 500ms
- [ ] ≥ 90% test coverage

### Phase 7 — Prediction
- [ ] Trajectory prediction ADE ≤ 2.0m at 5s horizon
- [ ] Multi-modal predictions generated for ≥ 90% of agents
- [ ] Risk scores correctly rank collision likelihood
- [ ] ≥ 90% test coverage

### Phase 8 — Planning
- [ ] Route planning finds valid path in ≤ 100ms
- [ ] Behavior planner handles ≥ 20 scenario types
- [ ] Motion planner generates kinematically feasible trajectories
- [ ] Fallback trajectory always available
- [ ] ≥ 90% test coverage

### Phase 9 — Control
- [ ] Lateral tracking RMSE ≤ 5cm
- [ ] Longitudinal speed tracking RMSE ≤ 0.5 m/s
- [ ] Control loop sustains 100Hz
- [ ] Emergency braking activated in ≤ 50ms
- [ ] ≥ 90% test coverage

### Phase 10 — Safety Platform
- [ ] Safety monitor detects ≥ 99% of injected faults
- [ ] Emergency stop completes in ≤ 50ms
- [ ] Safety envelope violations blocked with zero false negatives (in test suite)
- [ ] ODD boundary violations detected
- [ ] ≥ 90% test coverage

### Phase 11 — Digital Twin
- [ ] Vehicle twin dynamics error ≤ 5% vs. CARLA physics
- [ ] Sensor twin noise characteristics match specification
- [ ] Traffic twin generates realistic agent behavior
- [ ] ≥ 90% test coverage

### Phase 12 — Simulation Platform
- [ ] ≥ 100 scenarios executable in batch mode
- [ ] Replay system reproduces recorded sessions bit-accurately
- [ ] Metrics collected and aggregated automatically
- [ ] ≥ 90% test coverage

### Phase 13 — Validation Platform
- [ ] Automated test suite runs end-to-end without manual intervention
- [ ] Chaos testing discovers ≥ 5 previously unknown failure modes
- [ ] Performance benchmarks run and report automatically
- [ ] ≥ 90% test coverage

### Phase 14 — Fleet Platform
- [ ] Telemetry pipeline handles ≥ 100 vehicles concurrently
- [ ] OTA update delivered and verified on ≥ 10 simulated vehicles
- [ ] Remote diagnostics retrieves logs within 30 seconds
- [ ] ≥ 90% test coverage

### Phase 15 — Production Hardening
- [ ] 24-hour stress test passes with 0 crashes
- [ ] No critical/high security findings
- [ ] Memory usage stable (no leaks) over 24 hours
- [ ] Operational runbook covers all failure scenarios
- [ ] ≥ 90% test coverage

---

## 7. Traceability Matrix

> **Note**: This is the top-level traceability structure. Detailed per-requirement traceability will be maintained in `MASTER_KNOWLEDGE_GRAPH.md` and updated as components are implemented.

| Requirement Category | Component(s) | Test Category | Phase |
|---------------------|-------------|--------------|-------|
| NFR-PERF-* | core/kernel, core/event_bus, core/scheduler | Performance benchmarks | 2 |
| NFR-REL-* | core/health, core/lifecycle, safety/ | Reliability tests, fault injection | 2, 10, 13 |
| NFR-SAF-* | safety/*, core/health | Safety tests, fault injection | 10, 13 |
| NFR-SCA-* | core/event_bus, fleet/, simulation/ | Scalability tests | 2, 12, 14 |
| NFR-MNT-* | All components | Static analysis, coverage reports | All |
| NFR-SEC-* | core/messaging, fleet/ota | Security scans, pen testing | 2, 14, 15 |
| NFR-OBS-* | All components | Observability integration tests | 1+ |
| FR-FND-* | Build system, CI, docs | Build verification tests | 1 |
| FR-KRN-* | core/* | Unit, integration, performance | 2 |
| FR-VAL-* | hal/* | Unit, integration, compliance | 3 |
| FR-SEN-* | sensors/* | Unit, integration, simulation | 4 |
| FR-PER-* | perception/* | Unit, benchmark, simulation | 5 |
| FR-LOC-* | localization/* | Unit, accuracy benchmark | 6 |
| FR-PRD-* | prediction/* | Unit, accuracy benchmark | 7 |
| FR-PLN-* | planning/* | Unit, scenario simulation | 8 |
| FR-CTL-* | control/* | Unit, HIL (if available) | 9 |
| FR-SFT-* | safety/* | Fault injection, safety tests | 10 |
| FR-DTW-* | digital_twin/* | Fidelity validation | 11 |
| FR-SIM-* | simulation/* | Scenario execution tests | 12 |
| FR-VLD-* | validation/* | Meta-testing (tests of tests) | 13 |
| FR-FLT-* | fleet/* | Integration, load testing | 14 |
| FR-PRH-* | All components | Stress, security, performance | 15 |

---

*End of Master Requirements Document*
