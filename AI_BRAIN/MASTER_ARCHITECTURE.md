# UADOS — Master Architecture Document

> **Version**: 0.1.0  
> **Status**: Draft  
> **Last Updated**: 2026-05-30  
> **Owner**: UADOS Architecture Team

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Design Principles](#2-design-principles)
3. [Layer Architecture](#3-layer-architecture)
4. [Component Architecture](#4-component-architecture)
5. [Data Flow Architecture](#5-data-flow-architecture)
6. [Interface Contracts](#6-interface-contracts)
7. [Deployment Architecture](#7-deployment-architecture)
8. [Technology Stack](#8-technology-stack)
9. [Cross-Cutting Concerns](#9-cross-cutting-concerns)

---

## 1. Architecture Overview

UADOS employs a **layered microkernel architecture** where a minimal, safety-critical kernel manages component lifecycle, scheduling, and communication. All domain-specific functionality (perception, planning, control, etc.) is implemented as plugins that communicate through a zero-copy event bus.

```mermaid
graph TB
    subgraph "Fleet Layer"
        FT[Fleet Telemetry]
        OTA[OTA Updates]
        RD[Remote Diagnostics]
        FA[Fleet Analytics]
    end

    subgraph "Validation Layer"
        DT[Digital Twin]
        SIM[Simulation]
        VAL[Validation]
    end

    subgraph "Safety Layer"
        SM[Safety Monitor]
        FD[Fault Detection]
        ER[Emergency Response]
        SE[Safety Envelope]
    end

    subgraph "Autonomy Pipeline"
        PER[Perception]
        LOC[Localization]
        PRD[Prediction]
        PLN[Planning]
        CTL[Control]
    end

    subgraph "Sensor Layer"
        CAM[Camera]
        RAD[Radar]
        LID[LiDAR]
        GPS[GPS/GNSS]
        IMU[IMU]
        SF[Sensor Fusion]
    end

    subgraph "Abstraction Layer"
        VAPI[Vehicle API]
        DRV[Drivers]
        DSDK[Driver SDK]
    end

    subgraph "Kernel Layer"
        EB[Event Bus]
        SCH[Scheduler]
        HM[Health Monitor]
        LC[Lifecycle Mgr]
        PS[Plugin System]
        MSG[Messaging]
    end

    CAM --> SF
    RAD --> SF
    LID --> SF
    GPS --> SF
    IMU --> SF

    SF --> PER
    SF --> LOC
    PER --> PRD
    LOC --> PRD
    PRD --> PLN
    PLN --> CTL

    CTL --> VAPI
    VAPI --> DRV

    SM -.->|monitors| PER
    SM -.->|monitors| PLN
    SM -.->|monitors| CTL
    SE -.->|constrains| CTL
    FD -.->|watches| HM

    DT -.->|mirrors| DRV
    SIM -.->|tests| PER
    SIM -.->|tests| PLN

    EB --- SCH
    EB --- HM
    EB --- LC
    EB --- PS
    EB --- MSG

    FT -.->|collects| HM
    OTA -.->|updates| PS

    style EB fill:#1a1a2e,stroke:#e94560,color:#eee
    style SM fill:#1a1a2e,stroke:#e94560,color:#eee
    style SE fill:#1a1a2e,stroke:#e94560,color:#eee
```

---

## 2. Design Principles

### 2.1 Core Principles

| # | Principle | Description |
|---|-----------|-------------|
| P1 | **Microkernel** | Minimal trusted core; all domain logic in plugins |
| P2 | **Zero-Copy** | Shared-memory message passing on performance-critical paths |
| P3 | **Deterministic** | Priority-based scheduling with deadline guarantees |
| P4 | **Abstraction** | All hardware accessed through uniform driver interfaces |
| P5 | **Safety Independence** | Safety monitor is an independent subsystem with override authority |
| P6 | **Observable** | Every component emits structured metrics, logs, and traces |
| P7 | **Simulation-First** | All components testable in simulation before deployment |
| P8 | **Plugin Architecture** | Versioned interfaces, hot-reload, capability negotiation |
| P9 | **Fail-Safe** | Every failure mode has a defined safe response |
| P10 | **Reproducible** | Deterministic builds, reproducible test environments |

### 2.2 Dependency Rules

```mermaid
graph TD
    A["Fleet (Phase 14)"] --> B["Validation (Phase 13)"]
    B --> C["Simulation (Phase 12)"]
    C --> D["Digital Twin (Phase 11)"]
    D --> E["Safety (Phase 10)"]
    E --> F["Control (Phase 9)"]
    F --> G["Planning (Phase 8)"]
    G --> H["Prediction (Phase 7)"]
    H --> I["Localization (Phase 6)"]
    I --> J["Perception (Phase 5)"]
    J --> K["Sensors (Phase 4)"]
    K --> L["HAL (Phase 3)"]
    L --> M["Kernel (Phase 2)"]
    M --> N["Foundation (Phase 1)"]

    style N fill:#0d1117,stroke:#58a6ff,color:#c9d1d9
    style M fill:#0d1117,stroke:#58a6ff,color:#c9d1d9
    style E fill:#0d1117,stroke:#e94560,color:#c9d1d9
```

**Rule**: A layer may only depend on layers below it. No upward dependencies. The Safety layer is an exception — it monitors all layers but has no functional dependency on them.

---

## 3. Layer Architecture

### 3.1 Kernel Layer (Phase 2)

The kernel is the minimal trusted computing base. It provides:

```mermaid
classDiagram
    class Kernel {
        +init()
        +run()
        +shutdown()
        -scheduler: Scheduler
        -event_bus: EventBus
        -health_monitor: HealthMonitor
        -lifecycle_manager: LifecycleManager
        -plugin_system: PluginSystem
    }

    class EventBus {
        +publish(topic: string, msg: SharedPtr~Message~)
        +subscribe(topic: string, callback: Callback)
        +unsubscribe(topic: string, id: SubscriptionId)
        -shared_memory_pool: MemoryPool
        -topic_registry: HashMap
    }

    class Scheduler {
        +register_task(task: Task, priority: Priority, period: Duration)
        +unregister_task(id: TaskId)
        +run_cycle()
        -ready_queue: PriorityQueue
        -deadline_monitor: DeadlineMonitor
    }

    class HealthMonitor {
        +register_component(id: ComponentId, timeout: Duration)
        +heartbeat(id: ComponentId)
        +get_status(id: ComponentId): HealthStatus
        -watchdog_timers: HashMap
        -status_registry: HashMap
    }

    class LifecycleManager {
        +load(component: Component)
        +init(id: ComponentId)
        +start(id: ComponentId)
        +pause(id: ComponentId)
        +stop(id: ComponentId)
        +unload(id: ComponentId)
        -state_machine: StateMachine
    }

    class PluginSystem {
        +load_plugin(path: string): PluginId
        +unload_plugin(id: PluginId)
        +query_capability(cap: string): Vec~PluginId~
        -plugin_registry: HashMap
        -interface_versions: HashMap
    }

    Kernel --> EventBus
    Kernel --> Scheduler
    Kernel --> HealthMonitor
    Kernel --> LifecycleManager
    Kernel --> PluginSystem
```

#### Component Lifecycle State Machine

```mermaid
stateDiagram-v2
    [*] --> Loaded: load()
    Loaded --> Initialized: init()
    Initialized --> Running: start()
    Running --> Paused: pause()
    Paused --> Running: resume()
    Running --> Stopping: stop()
    Paused --> Stopping: stop()
    Stopping --> Stopped: cleanup complete
    Stopped --> [*]: unload()
    
    Running --> Error: fault detected
    Initialized --> Error: init failure
    Error --> Stopping: recovery failed
    Error --> Initialized: recovery succeeded
```

### 3.2 Vehicle Abstraction Layer (Phase 3)

```mermaid
classDiagram
    class IVehicleDriver {
        <<interface>>
        +init(config: DriverConfig): Status
        +start(): Status
        +stop(): Status
        +read_state(): VehicleState
        +write_command(cmd: VehicleCommand): Status
        +get_capabilities(): DriverCapabilities
        +get_status(): DriverStatus
    }

    class VehicleState {
        +position: Position3D
        +velocity: Velocity3D
        +acceleration: Acceleration3D
        +orientation: Quaternion
        +steering_angle: float
        +wheel_speeds: float[4]
        +timestamp: Timestamp
    }

    class VehicleCommand {
        +steering_angle: float
        +throttle: float [0.0, 1.0]
        +brake: float [0.0, 1.0]
        +gear: GearPosition
        +timestamp: Timestamp
    }

    class CARLADriver {
        +init(config: DriverConfig): Status
        +start(): Status
        +stop(): Status
        +read_state(): VehicleState
        +write_command(cmd: VehicleCommand): Status
    }

    class CANBusDriver {
        +init(config: DriverConfig): Status
        +start(): Status
        +stop(): Status
        +read_state(): VehicleState
        +write_command(cmd: VehicleCommand): Status
    }

    IVehicleDriver <|.. CARLADriver
    IVehicleDriver <|.. CANBusDriver
    IVehicleDriver --> VehicleState
    IVehicleDriver --> VehicleCommand
```

### 3.3 Sensor Layer (Phase 4)

```mermaid
classDiagram
    class ISensor {
        <<interface>>
        +init(config: SensorConfig): Status
        +start(): Status
        +stop(): Status
        +read(): SensorData
        +get_calibration(): Calibration
        +get_health(): SensorHealth
    }

    class SensorFusion {
        +add_source(sensor: ISensor)
        +remove_source(id: SensorId)
        +update(): FusedState
        -ekf: ExtendedKalmanFilter
        -sources: Vec~ISensor~
    }

    class CameraDriver {
        +read(): ImageFrame
    }

    class LiDARDriver {
        +read(): PointCloud
    }

    class RadarDriver {
        +read(): RadarScan
    }

    class GPSDriver {
        +read(): GPSFix
    }

    class IMUDriver {
        +read(): IMUReading
    }

    ISensor <|.. CameraDriver
    ISensor <|.. LiDARDriver
    ISensor <|.. RadarDriver
    ISensor <|.. GPSDriver
    ISensor <|.. IMUDriver
    SensorFusion --> ISensor
```

### 3.4 Autonomy Pipeline (Phases 5–9)

```mermaid
graph LR
    subgraph "Perception (Phase 5)"
        DET[Detection]
        CLS[Classification]
        TRK[Tracking]
        SEG[Segmentation]
        LAN[Lane Detection]
        SIG[Sign Recognition]
        TFL[Traffic Light]
    end

    subgraph "Localization (Phase 6)"
        GF[GPS Fusion]
        VL[Visual Localization]
        SL[SLAM]
        HDM[HD Map Query]
        PE[Pose Estimation]
    end

    subgraph "Prediction (Phase 7)"
        TP[Trajectory Prediction]
        BP[Behavior Prediction]
        RE[Risk Estimation]
    end

    subgraph "Planning (Phase 8)"
        SP[Strategic Planner]
        BPL[Behavior Planner]
        MP[Motion Planner]
    end

    subgraph "Control (Phase 9)"
        LC[Lateral Control]
        LGC[Longitudinal Control]
        AC[Actuator Commands]
    end

    DET --> TRK
    CLS --> TRK
    TRK --> TP
    TRK --> BP
    LAN --> BPL
    SIG --> BPL
    TFL --> BPL

    GF --> PE
    VL --> PE
    SL --> PE
    HDM --> PE
    PE --> TP
    PE --> SP

    TP --> RE
    BP --> RE
    RE --> BPL

    SP --> BPL
    BPL --> MP
    MP --> LC
    MP --> LGC
    LC --> AC
    LGC --> AC
```

### 3.5 Safety Layer (Phase 10)

```mermaid
graph TB
    subgraph "Safety Platform"
        SM[Safety Monitor]
        FDI[Fault Detection & Isolation]
        SEV[Safety Envelope Validator]
        ERS[Emergency Response System]
        ODD[ODD Monitor]
        SAL[Safety Audit Logger]
    end

    subgraph "Monitored Systems"
        P[Perception]
        L[Localization]
        PR[Prediction]
        PL[Planning]
        C[Control]
    end

    SM -->|monitors| P
    SM -->|monitors| L
    SM -->|monitors| PR
    SM -->|monitors| PL
    SM -->|monitors| C

    SEV -->|constrains| C
    FDI -->|detects faults in| P
    FDI -->|detects faults in| L
    ODD -->|checks boundaries| SM

    SM -->|triggers| ERS
    FDI -->|escalates to| SM
    SEV -->|violation triggers| ERS
    ODD -->|out-of-domain triggers| ERS

    SM -->|records to| SAL
    ERS -->|records to| SAL

    style SM fill:#2d1b1b,stroke:#e94560,color:#eee
    style ERS fill:#2d1b1b,stroke:#e94560,color:#eee
    style SEV fill:#2d1b1b,stroke:#e94560,color:#eee
```

---

## 4. Component Architecture

### 4.1 Event Bus Architecture

The event bus is the backbone of inter-component communication. It uses a **publish-subscribe** model with **zero-copy shared memory** for high-throughput, low-latency data transfer.

```mermaid
sequenceDiagram
    participant S as Sensor Driver
    participant EB as Event Bus
    participant P as Perception
    participant SM as Safety Monitor

    S->>EB: publish("sensor/camera/0", frame_ptr)
    Note over EB: Zero-copy: only pointer transferred
    EB->>P: deliver("sensor/camera/0", frame_ptr)
    EB->>SM: deliver("sensor/camera/0", frame_ptr)
    P->>EB: publish("perception/detections", detections_ptr)
    EB->>SM: deliver("perception/detections", detections_ptr)
```

**Key Design Decisions:**
- **Shared Memory Pool**: Pre-allocated at startup, no runtime allocation
- **Lock-Free Queues**: SPSC (single-producer, single-consumer) queues per subscription
- **Topic-Based Routing**: Hierarchical topic names (e.g., `sensor/camera/0/image`)
- **QoS Policies**: Configurable per-topic (reliable, best-effort, last-value)
- **Message Lifecycle**: Reference-counted, automatically returned to pool

### 4.2 Scheduler Architecture

```mermaid
sequenceDiagram
    participant SCH as Scheduler
    participant T1 as "Perception (P=High, T=100ms)"
    participant T2 as "Planning (P=High, T=50ms)"
    participant T3 as "Logging (P=Low, T=1000ms)"

    loop Every Tick
        SCH->>SCH: Check deadlines
        SCH->>T2: Execute (highest priority, shortest period)
        T2-->>SCH: Complete (12ms)
        SCH->>T1: Execute
        T1-->>SCH: Complete (45ms)
        SCH->>T3: Execute (if time remains)
        T3-->>SCH: Complete (5ms)
        SCH->>SCH: Log timing statistics
    end
```

**Scheduling Algorithm**: Rate-Monotonic Scheduling (RMS) with deadline monitoring. Tasks that miss deadlines are reported to the Health Monitor.

### 4.3 Plugin Architecture

```mermaid
graph TB
    subgraph "Plugin System"
        PR[Plugin Registry]
        IL[Interface Loader]
        VM[Version Manager]
        SB[Sandbox]
    end

    subgraph "Plugin Interface"
        PI["IPlugin"]
        PI1["name(): string"]
        PI2["version(): SemVer"]
        PI3["dependencies(): Vec&lt;Dep&gt;"]
        PI4["init(ctx: PluginContext)"]
        PI5["start()"]
        PI6["stop()"]
    end

    subgraph "Loaded Plugins"
        P1[CARLA Driver Plugin]
        P2[YOLO Detector Plugin]
        P3[MPC Controller Plugin]
    end

    IL --> PI
    PR --> P1
    PR --> P2
    PR --> P3
    VM -->|version check| IL

    P1 -.->|implements| PI
    P2 -.->|implements| PI
    P3 -.->|implements| PI
```

---

## 5. Data Flow Architecture

### 5.1 Main Autonomy Loop

```mermaid
graph LR
    A[Sensors] -->|Raw Data| B[Sensor Fusion]
    B -->|Fused State| C[Perception]
    C -->|Objects, Lanes, Signs| D[Prediction]
    B -->|Fused State| E[Localization]
    E -->|Pose| D
    D -->|Trajectories, Risks| F[Planning]
    E -->|Pose| F
    F -->|Planned Trajectory| G[Control]
    G -->|Commands| H[Vehicle API]
    H -->|Actuator Signals| I[Vehicle Hardware]

    J[Safety Monitor] -.->|monitors all| C
    J -.->|monitors all| D
    J -.->|monitors all| F
    J -.->|constrains| G
    J -.->|can override| H

    style J fill:#2d1b1b,stroke:#e94560,color:#eee
```

### 5.2 Data Types and Flow Rates

| Data Flow | Type | Size (approx) | Rate | Latency Budget |
|-----------|------|---------------|------|---------------|
| Camera → Perception | ImageFrame (1920×1080 RGB) | 6 MB | 30 Hz | 33ms |
| LiDAR → Perception | PointCloud (300K points) | 3.6 MB | 10 Hz | 100ms |
| Radar → Fusion | RadarScan (64 targets) | 4 KB | 20 Hz | 50ms |
| GPS → Localization | GPSFix | 128 B | 10 Hz | 100ms |
| IMU → Fusion | IMUReading | 64 B | 200 Hz | 5ms |
| Perception → Prediction | DetectedObjects (100 max) | 50 KB | 10 Hz | 10ms |
| Localization → Planning | Pose6D | 128 B | 50 Hz | 2ms |
| Prediction → Planning | PredictedTrajectories | 200 KB | 10 Hz | 10ms |
| Planning → Control | PlannedTrajectory | 10 KB | 10 Hz | 5ms |
| Control → HAL | VehicleCommand | 64 B | 100 Hz | 1ms |

### 5.3 Recording and Replay

```mermaid
graph TB
    subgraph "Online (Vehicle)"
        S[Sensors] --> R[Recorder]
        EB[Event Bus] --> R
        R --> FS[File Storage]
    end

    subgraph "Offline (Workstation)"
        FS --> RP[Replay Player]
        RP --> EB2[Event Bus]
        EB2 --> PIPE[Autonomy Pipeline]
        PIPE --> M[Metrics Collector]
        M --> DB[Results Database]
    end
```

---

## 6. Interface Contracts

### 6.1 Component Interface (Base)

Every UADOS component implements this interface:

```cpp
// uados/core/component.hpp
namespace uados::core {

class IComponent {
public:
    virtual ~IComponent() = default;

    // Lifecycle
    virtual Status init(const Config& config) = 0;
    virtual Status start() = 0;
    virtual Status stop() = 0;

    // Identity
    virtual std::string_view name() const = 0;
    virtual Version version() const = 0;

    // Health
    virtual HealthStatus health() const = 0;

    // Configuration
    virtual void reconfigure(const Config& config) = 0;
};

} // namespace uados::core
```

### 6.2 Event Bus Interface

```cpp
// uados/core/event_bus.hpp
namespace uados::core {

class IEventBus {
public:
    virtual ~IEventBus() = default;

    // Publish a message to a topic (zero-copy)
    virtual void publish(std::string_view topic,
                         SharedPtr<const Message> msg) = 0;

    // Subscribe to a topic
    virtual SubscriptionId subscribe(
        std::string_view topic,
        std::function<void(SharedPtr<const Message>)> callback,
        QoSPolicy qos = QoSPolicy::BestEffort) = 0;

    // Unsubscribe
    virtual void unsubscribe(SubscriptionId id) = 0;

    // Query
    virtual std::vector<std::string> list_topics() const = 0;
    virtual size_t subscriber_count(std::string_view topic) const = 0;
};

} // namespace uados::core
```

### 6.3 Vehicle Driver Interface

```cpp
// uados/hal/driver.hpp
namespace uados::hal {

class IVehicleDriver {
public:
    virtual ~IVehicleDriver() = default;

    virtual Status init(const DriverConfig& config) = 0;
    virtual Status start() = 0;
    virtual Status stop() = 0;

    virtual VehicleState read_state() = 0;
    virtual Status write_command(const VehicleCommand& cmd) = 0;

    virtual DriverCapabilities capabilities() const = 0;
    virtual DriverStatus status() const = 0;
};

} // namespace uados::hal
```

### 6.4 Sensor Interface

```cpp
// uados/sensors/sensor.hpp
namespace uados::sensors {

class ISensor {
public:
    virtual ~IComponent() = default;

    virtual Status init(const SensorConfig& config) = 0;
    virtual Status start() = 0;
    virtual Status stop() = 0;

    virtual SensorData read() = 0;
    virtual Calibration calibration() const = 0;
    virtual SensorHealth health() const = 0;
    virtual SensorInfo info() const = 0;
};

} // namespace uados::sensors
```

### 6.5 Plugin Interface

```cpp
// uados/core/plugin.hpp
namespace uados::core {

class IPlugin {
public:
    virtual ~IPlugin() = default;

    virtual std::string_view name() const = 0;
    virtual Version version() const = 0;
    virtual std::vector<Dependency> dependencies() const = 0;

    virtual Status init(PluginContext& ctx) = 0;
    virtual Status start() = 0;
    virtual Status stop() = 0;
};

// Plugin entry point macro
#define UADOS_PLUGIN(PluginClass) \
    extern "C" IPlugin* create_plugin() { return new PluginClass(); } \
    extern "C" void destroy_plugin(IPlugin* p) { delete p; }

} // namespace uados::core
```

---

## 7. Deployment Architecture

### 7.1 Single Vehicle Deployment

```mermaid
graph TB
    subgraph "Compute Unit (NVIDIA Orin / x86)"
        subgraph "Process: uados-core"
            K[Kernel]
            EB[Event Bus]
            SCH[Scheduler]
        end

        subgraph "Process: uados-perception"
            PER[Perception Pipeline]
            GPU[GPU Inference]
        end

        subgraph "Process: uados-planning"
            LOC[Localization]
            PRD[Prediction]
            PLN[Planning]
            CTL[Control]
        end

        subgraph "Process: uados-safety"
            SM[Safety Monitor]
            SE[Safety Envelope]
        end

        subgraph "Process: uados-recorder"
            REC[Data Recorder]
        end
    end

    subgraph "Sensor Hardware"
        C1[Camera 1..N]
        R1[Radar 1..N]
        L1[LiDAR 1..N]
        G1[GPS/GNSS]
        I1[IMU]
    end

    subgraph "Vehicle Bus"
        CAN[CAN Bus]
        ETH[Ethernet]
    end

    C1 --> PER
    R1 --> PER
    L1 --> PER
    G1 --> LOC
    I1 --> LOC

    CTL --> CAN
    CTL --> ETH

    SM -.->|shared memory| PER
    SM -.->|shared memory| PLN

    K --- PER
    K --- PLN
    K --- SM
    K --- REC
```

### 7.2 Fleet Deployment

```mermaid
graph TB
    subgraph "Cloud Infrastructure"
        subgraph "Fleet Services"
            TI[Telemetry Ingestion]
            OTA[OTA Service]
            DA[Diagnostics API]
            AN[Analytics Engine]
        end

        subgraph "Data Infrastructure"
            TS[Time-Series DB]
            OBJ[Object Storage]
            MSG[Message Queue]
        end

        subgraph "Simulation Cluster"
            SIM[Simulation Orchestrator]
            W1[Worker 1..N]
        end
    end

    subgraph "Vehicle Fleet"
        V1[Vehicle 1]
        V2[Vehicle 2]
        VN[Vehicle N]
    end

    V1 -->|telemetry| TI
    V2 -->|telemetry| TI
    VN -->|telemetry| TI

    TI --> TS
    TI --> MSG

    OTA -->|updates| V1
    OTA -->|updates| V2
    OTA -->|updates| VN

    DA -->|queries| V1
    DA -->|queries| V2
    AN --> TS
    SIM --> W1
```

---

## 8. Technology Stack

### 8.1 Core Technologies

| Layer | Technology | Version | Rationale |
|-------|-----------|---------|-----------|
| **Language (runtime)** | C++20 | GCC 13+ / Clang 17+ | Performance, determinism, zero-cost abstractions |
| **Language (tooling)** | Python 3.12 | 3.12+ | ML ecosystem, scripting, test automation |
| **Build** | CMake | 3.28+ | Cross-platform, widely supported |
| **Package Manager** | Conan 2 | 2.x | C++ dependency management |
| **Python Packages** | pip + pyproject.toml | — | Standard Python packaging |
| **Serialization (hot)** | FlatBuffers | 24.x | Zero-copy deserialization |
| **Serialization (cold)** | Protocol Buffers | 3.x | Strong typing, broad ecosystem |
| **ML Inference** | ONNX Runtime | 1.18+ | Hardware-agnostic, broad model support |
| **ML Training** | PyTorch | 2.x | Dominant in research, strong ecosystem |
| **Maps** | Lanelet2 | latest | Open-source, automotive-grade |
| **Simulation** | CARLA | 0.9.15+ | Open-source, realistic rendering |
| **Traffic Sim** | SUMO | 1.x | Microscopic traffic simulation |

### 8.2 Infrastructure Technologies

| Purpose | Technology | Rationale |
|---------|-----------|-----------|
| **Metrics** | Prometheus | Industry standard, pull-based |
| **Tracing** | OpenTelemetry | Vendor-neutral, comprehensive |
| **Dashboards** | Grafana | Flexible, supports Prometheus |
| **Logging** | spdlog (C++) + structlog (Python) | High-performance structured logging |
| **CI/CD** | GitHub Actions | Integrated with repository |
| **Containers** | Docker | Reproducible environments |
| **Docs (C++)** | Doxygen | Standard for C++ API docs |
| **Docs (Python)** | Sphinx | Standard for Python docs |
| **Diagrams** | Mermaid | Version-controlled, text-based |

### 8.3 Key Libraries

| Library | Purpose | License |
|---------|---------|---------|
| Eigen3 | Linear algebra, matrix operations | MPL2 |
| OpenCV | Image processing, basic CV | Apache 2.0 |
| PCL | Point cloud processing | BSD |
| Ceres Solver | Non-linear optimization (SLAM, calibration) | Apache 2.0 |
| Google Test | C++ unit testing | BSD-3 |
| Google Benchmark | C++ micro-benchmarking | Apache 2.0 |
| pybind11 | C++/Python bindings | BSD |
| nlohmann/json | JSON parsing (config) | MIT |
| yaml-cpp | YAML parsing (config) | MIT |
| fmt | String formatting | MIT |
| spdlog | Structured logging | MIT |
| abseil-cpp | Utilities, containers | Apache 2.0 |
| gRPC | Fleet communication | Apache 2.0 |

---

## 9. Cross-Cutting Concerns

### 9.1 Error Handling Strategy

```
Error Classification:
├── Recoverable Errors
│   ├── Sensor timeout → retry with backoff
│   ├── GPS signal loss → switch to dead reckoning
│   └── Plugin crash → restart plugin
├── Degraded Operation
│   ├── Camera failure → radar/LiDAR-only perception
│   ├── HD map unavailable → local SLAM only
│   └── Connectivity loss → autonomous operation continues
└── Critical Errors (→ Safe Stop)
    ├── Multiple sensor failures → minimum risk condition
    ├── Planning failure → execute fallback trajectory
    ├── Control loop timeout → emergency brake
    └── Safety monitor failure → immediate safe stop
```

### 9.2 Configuration Management

```yaml
# Example: Vehicle Configuration
vehicle:
  name: "carla_simulation"
  driver: "uados::hal::CARLADriver"
  capabilities:
    max_steering_angle: 70.0  # degrees
    max_speed: 50.0           # m/s
    max_acceleration: 4.0     # m/s²
    max_deceleration: 8.0     # m/s²

sensors:
  - type: camera
    driver: "uados::sensors::CARLACamera"
    config:
      resolution: [1920, 1080]
      fov: 90
      fps: 30
      mount:
        position: [2.0, 0.0, 1.5]  # x, y, z in vehicle frame
        rotation: [0.0, 0.0, 0.0]  # roll, pitch, yaw

  - type: lidar
    driver: "uados::sensors::CARLALiDAR"
    config:
      channels: 64
      range: 100.0
      points_per_second: 300000
      rotation_frequency: 10

scheduler:
  perception:
    priority: 8
    period_ms: 100
  planning:
    priority: 9
    period_ms: 50
  control:
    priority: 10
    period_ms: 10
  safety:
    priority: 11  # highest
    period_ms: 10
```

### 9.3 Logging Standard

All log entries follow this structure:

```json
{
  "timestamp": "2026-05-30T15:30:00.123456Z",
  "level": "INFO",
  "component": "perception.detection",
  "thread_id": 12345,
  "message": "Detection cycle complete",
  "data": {
    "objects_detected": 12,
    "inference_time_ms": 23.4,
    "frame_id": 98765
  },
  "trace_id": "abc123def456"
}
```

### 9.4 Metrics Standard

All metrics use OpenTelemetry naming conventions:

```
uados.perception.detection.inference_time_ms      (histogram)
uados.perception.detection.objects_count           (gauge)
uados.planning.cycle_time_ms                       (histogram)
uados.control.tracking_error.lateral_m             (gauge)
uados.control.tracking_error.longitudinal_ms       (gauge)
uados.safety.envelope_violations_total             (counter)
uados.event_bus.messages_total                     (counter)
uados.event_bus.latency_us                         (histogram)
uados.scheduler.deadline_misses_total              (counter)
uados.health.component_status                      (gauge, labeled)
```

### 9.5 Memory Management Strategy

```
Hot Path (Real-time):
├── Pre-allocated memory pools
├── Fixed-size ring buffers for sensor data
├── Lock-free SPSC queues
└── No malloc/free during runtime

Cold Path (Non-real-time):
├── Standard allocators acceptable
├── Smart pointers for ownership
└── RAII for all resources

Shared Memory:
├── Named shared memory regions per topic
├── Reference-counted message buffers
├── Memory-mapped files for large data (point clouds, images)
└── Automatic cleanup on process exit
```

---

*End of Master Architecture Document*
