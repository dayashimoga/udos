# AI Context Document (AIPBF v4.0)

> **Generated**: 2026-06-02
> **Purpose**: LLM-optimized project understanding. A completely different AI model should be able to read ONLY this file and understand the project well enough to continue development accurately.

---

## Project Identity

- **Project Type**: Autonomous Driving Operating System
- **Project Domain**: Autonomous Vehicles & Robotic Systems
- **Primary Purpose**: Failsafe real-time vehicle scheduling, fusion, path planning, and envelope controls.
- **Confidence**: HIGH
- **Primary Languages**: C++, Markdown, Python, YAML
- **Build Tooling**: Conan, CMake
- **Total LOC**: 25235

---

## System Intent Map

### Primary Goal:
Safely navigate autonomous vehicles in dynamic environments.

### System Mission:
1. Acquire sensor data (IMU, GPS, LiDAR, Camera)
2. Fuse sensor streams (EKF state filters)
3. Localize vehicle (Pose & Odometry)
4. Predict actor behavior (Trajectory estimates)
5. Plan trajectory (Obstacle-avoidance motion planner)
6. Generate control commands (Stanley lateral controller, PID speed loops)
7. Monitor safety boundaries (Emergency braking, envelope constraints)
8. Execute fallback actions (CAN hardware shutdown, safe harbor maneuvers)

---

## Runtime Data Flow

```mermaid
graph LR
    Sensors[1. Sensors] -->|Raw feeds| Perception[2. Perception]
    Perception -->|Fused streams| Localization[3. Localization]
    Localization -->|Pose & Velocity| Prediction[4. Prediction]
    Prediction -->|Actor trajectories| Planning[5. Planning]
    Planning -->|Steer & Throttle commands| Control[6. Control]
    Control -->|Actuator commands| Safety[7. Safety Envelope]
    Safety -->|Plausible commands| HAL[8. HAL Actuators]
```

---

## Architecture Rules

> [!IMPORTANT]
> **Strict Robotics Structural Boundaries**
> 1. **Perception never directly controls actuators**: Perception must output track/object states; it is forbidden to bypass the planner and send direct CAN commands.
> 2. **Planning cannot bypass the safety layer**: All planned trajectories must pass through safety envelope collision checks before control execution.
> 3. **All subsystem commands pass through the EventBus**: Explicit decoupled IPC model. Direct inline cross-imports between core modules are prohibited.
> 4. **Safety may override any subsystem**: Failsafe watchdogs and emergency braking can override planned trajectories at any step.
> 5. **No module directly accesses hardware except HAL**: Subsystems must interact with sensors and actuators through HAL abstractions only.

---

## Known Constraints

- **Zero Heap Allocations on Realtime Hot Path**: All control loop steps must use pre-allocated static memory blocks (NFR-PERF-010).
- **Hard Realtime Deadlines**: System-wide control loop frequencies must sustain >= 100Hz with watchdog alerts (NFR-PERF-004).
- **Deterministic Scheduling**: Scheduler prioritizes failsafe critical execution rings (FR-KRN-003).
- **ASIL-D Independence**: Safety monitors run isolated from user control space (NFR-SAF-001).

---

## VERIFIED_FACTS VS AI_INFERENCES

### VERIFIED_FACTS (100% Proven on Disk)
- **Directory Layout**: Subsystem folders verified on disk.
- **Source Files**: 429 source files, 25 test files present.
- **Build Configurations**: Conan, CMake active and verified.
- **Static Security**: Static analyzer results completed.

### AI_INFERENCES (Inferred from Static Structures)
- **Architecture Import Graph**: Derived through import dependencies (build-time, not runtime).
- **Runtime flow**: Thread orchestration paths are inferred from standard boot sequences.
- **Performance budgets**: Latency boundaries are simulated targets; no physical CPU profiling data verified.

---

## Quick Navigation

| Document | Purpose |
|:---|:---|
| [PROJECT_BRAIN.md](./PROJECT_BRAIN.md) | Master index with all section summaries |
| [AI_HANDOFF.md](./AI_HANDOFF.md) | Context restoration & development contract |
| [MASTER_ARCHITECTURE.md](./MASTER_ARCHITECTURE.md) | Full architecture with Mermaid diagrams |
| [MASTER_REQUIREMENTS.md](./MASTER_REQUIREMENTS.md) | Requirements traceability matrix |
| [MASTER_KNOWLEDGE_GRAPH.md](./MASTER_KNOWLEDGE_GRAPH.md) | Domain models, messages, interfaces |
| [MASTER_SECURITY.md](./MASTER_SECURITY.md) | Security posture & SAST findings |
| [MASTER_TESTING.md](./MASTER_TESTING.md) | Test registry & coverage evidence |
| [MASTER_RISKS.md](./MASTER_RISKS.md) | Risk registry & failure modes |
| [MASTER_PROGRESS.md](./MASTER_PROGRESS.md) | Feature lifecycle & production readiness |
| [MASTER_VALIDATION_STATUS.md](./MASTER_VALIDATION_STATUS.md) | Change impact & drift detection |
