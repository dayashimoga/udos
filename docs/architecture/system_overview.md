# UADOS — Architecture System Overview

> **Classification**: Open Source Autonomous Stack  
> **Status**: Active / Production-Ready  
> **Target Audience**: Core Engineers & Integrators  

---

## 1. High-Level Architecture Design

UADOS (Universal Autonomous Driving Operating System) is designed using a **modular microkernel-inspired architecture**. Instead of a monolithic control loop, functions are split into decoupled subsystems running on top of a highly responsive **Event Bus** scheduler.

```mermaid
graph TD
    %% Define System Layers
    subgraph Hardware_Layer [HAL & Sensor Drivers]
        A[GPS Driver] -->|GPSFixEvent| SF[Sensor Fusion EKF]
        B[IMU Driver] -->|IMUReadingEvent| SF
        C[LiDAR Driver] -->|PointCloudEvent| PER[Perception Pipeline]
        D[Camera Driver] -->|ImageEvent| PER
        E[CAN Bus DBW] <-->|CANFrameEvent| VAL[Vehicle Abstraction Layer]
    end

    subgraph OS_Kernel [UADOS Microkernel Core]
        KB[Event Bus Router]
        KS[Real-time Task Scheduler]
        KL[Lifecycle Manager]
        KM[Memory Pool Allocator]
    end

    subgraph Core_Intelligence [Perception, Planning & Control]
        SF -->|OdometryStateEvent| LOC[Localization Engine]
        PER -->|DetectedObjectsEvent| TRA[Trajectory Predictor]
        LOC -->|PoseStateEvent| TRA
        TRA -->|PredictedStatesEvent| PLN[Strategic/Motion Planner]
        PLN -->|ReferenceTrajectoryEvent| CON[Stanley Control Loop]
        CON -->|SteeringCommandEvent| VAL
    end

    subgraph Operations [Safety & Fleet Management]
        SAF[Safety Envelope Monitor] -->|FaultCondition| ERS[Emergency Response System]
        ERS -->|OverrideCommand| VAL
        FLT[Fleet Telemetry Engine] <-->|OTA / Telemetry| CLD[Cloud Management Platform]
    end

    %% Apply visual styling
    classDef layer fill:#1e1e24,stroke:#3a3a4a,stroke-width:2px,color:#fff;
    classDef kernel fill:#1a3a5f,stroke:#2b6cb0,stroke-width:2px,color:#fff;
    classDef safety fill:#5f1a1a,stroke:#c53030,stroke-width:2px,color:#fff;
    
    class OS_Kernel kernel;
    class Operations safety;
```

---

## 2. Platform Core Subsystems

### A. UADOS Microkernel Core
The microkernel manages deterministic execution, logging, configuration, memory, and the IPC bus.
- **Event Bus (`core/event_bus/`)**: A thread-safe, lock-free lockless event dispatch ring-buffer that enables zero-copy messaging between independent C++ components.
- **Scheduler (`core/scheduler/`)**: Multi-threaded real-time executor supporting priority scheduling, rate limiting, and execution timing guarantees.
- **Health / Lifecycle (`core/health/`, `core/lifecycle/`)**: System lifecycle state managers ensuring safe transition states (`Uninitialized` → `Initialized` → `Running` → `Stopped` → `Panic`).

### B. Hardware Abstraction Layer (HAL)
Standardizes interface boundaries between actual vehicle drive-by-wire (DBW) systems and higher-level path planning.
- Supports physical `SocketCAN` CAN frame streaming.
- Built-in simulation driver interface for direct CARLA integration.

### C. Sensory & Perception Engines
Handles EKF (Extended Kalman Filtering) sensor fusion, point-cloud transformations, visual classification, and HD lane-map matching.
- **Sensor Fusion (`sensors/fusion/`)**: Fuses coordinate tracking from GPS and IMU sensors to yield highly accurate metric position estimates.
- **Inference Engine (`perception/detection/`)**: Parses incoming vision frames via ONNX networks to localize and label roadway obstacles.

### D. Planning & Stanley Control
Translates the mapped roadway and obstacle predictions into standard steer/throttle commands.
- **Strategic Planner (`planning/strategic/`)**: Generates high-level highway corridor waypoints.
- **Stanley Control Loop (`control/loops/`)**: Uses lateral cross-track steering error geometry to drive physical actuators.

### E. Emergency Envelope
A fail-operational safety layer that continuously evaluates hardware metrics.
- **Emergency Response System (`safety/emergency/`)**: Instantly overrides nominal controls with MRC (Minimum Risk Maneuver) fallback trajectories when a critical fault is discovered.
