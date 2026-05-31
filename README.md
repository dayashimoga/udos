# 🚗 UADOS — Universal Autonomous Driving Operating System

<div align="center">

![Phase](https://img.shields.io/badge/Phase-0%20Requirements-blue)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![C++](https://img.shields.io/badge/C%2B%2B-20-orange)
![Python](https://img.shields.io/badge/Python-3.12-blue)
![Safety](https://img.shields.io/badge/Safety-ASIL--B-red)

**A modular, safety-critical, platform-agnostic autonomous driving platform.**

*Simulation → RC Car → Production Vehicle*

</div>

---

## 🎯 Vision

UADOS is a **Universal Autonomous Driving Operating System** designed to operate on multiple vehicle platforms through a driver abstraction architecture. It provides a complete autonomy stack from sensor fusion through perception, prediction, planning, and control — with an independent safety platform that cannot be overridden by any other subsystem.

## 🏗️ Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                    FLEET PLATFORM (Phase 14)                      │
│   Telemetry · OTA · Remote Diagnostics · Fleet Analytics          │
├──────────────────────────────────────────────────────────────────┤
│               DIGITAL TWIN & SIMULATION (Phase 11-12)             │
│   Vehicle Twins · Sensor Twins · Scenario Engine · Replay         │
├──────────────────────────────────────────────────────────────────┤
│                  SAFETY PLATFORM (Phase 10)                       │
│   Safety Monitor · Fault Detection · Emergency Response           │
├──────────┬──────────┬──────────┬──────────┬──────────────────────┤
│ PERCEP.  │ LOCAL.   │ PREDICT. │ PLANNING │ CONTROL              │
│ (Ph. 5)  │ (Ph. 6)  │ (Ph. 7)  │ (Ph. 8)  │ (Ph. 9)             │
├──────────┴──────────┴──────────┴──────────┴──────────────────────┤
│                   SENSOR PLATFORM (Phase 4)                       │
│   Camera · Radar · LiDAR · GPS · IMU · Sensor Fusion             │
├──────────────────────────────────────────────────────────────────┤
│             VEHICLE ABSTRACTION LAYER (Phase 3)                   │
│   Vehicle API · CARLA Driver · RC Car Driver · CAN Driver         │
├──────────────────────────────────────────────────────────────────┤
│                VEHICLE OS KERNEL (Phase 2)                        │
│   Event Bus · Scheduler · Health Monitor · Plugin System          │
├──────────────────────────────────────────────────────────────────┤
│              FOUNDATION PLATFORM (Phase 1)                        │
│   Build · CI/CD · Docs · Observability · Dev Environment          │
└──────────────────────────────────────────────────────────────────┘
```

## 🔑 Key Features

- **Microkernel Architecture** — Minimal trusted core; all subsystems are plugins
- **Zero-Copy Event Bus** — Sub-microsecond shared-memory message passing
- **Driver Abstraction** — One API, multiple vehicles (simulation, RC car, production)
- **Safety Independence** — Separate safety monitor with override authority
- **Simulation-First** — Every component validated in CARLA before hardware
- **Plugin System** — Hot-swappable, versioned, dynamically-loaded extensions
- **Observable by Default** — OpenTelemetry metrics, traces, and structured logs
- **73 Components** — Covering perception, localization, prediction, planning, control, and beyond

## 🚀 Vehicle Platforms

| Platform | Purpose | Hardware |
|----------|---------|----------|
| **CARLA Simulation** | Primary development & validation | NVIDIA GPU (RTX 3060+) |
| **1/10 RC Car** | Physical validation, safe testing | Jetson Orin Nano, RealSense, RPLiDAR |
| **Production Vehicle** | Real-world deployment | Jetson AGX Orin, industrial sensors, CAN bus |

## 📂 Project Structure

```
uados/
├── AI_BRAIN/          # Context preservation framework (11 master docs)
├── core/              # Phase 2: Vehicle OS kernel
├── hal/               # Phase 3: Vehicle abstraction layer
├── sensors/           # Phase 4: Sensor platform
├── perception/        # Phase 5: Perception pipeline
├── localization/      # Phase 6: Localization engine
├── prediction/        # Phase 7: Prediction system
├── planning/          # Phase 8: Planning pipeline
├── control/           # Phase 9: Control system
├── safety/            # Phase 10: Safety platform
├── digital_twin/      # Phase 11: Digital twin platform
├── simulation/        # Phase 12: Simulation platform
├── validation/        # Phase 13: Validation framework
├── fleet/             # Phase 14: Fleet management
├── tools/             # CLI, dashboard, code generators
├── docs/              # Documentation
├── scripts/           # Build & deployment scripts
└── configs/           # Configuration files
```

## 🛠️ Technology Stack

| Category | Technology |
|----------|-----------|
| **Runtime** | C++20 (GCC 13+, Clang 17+) |
| **Tooling/ML** | Python 3.12 |
| **Build** | CMake 3.28+, Conan 2 |
| **Serialization** | FlatBuffers (hot path), Protobuf (cold path) |
| **ML Inference** | ONNX Runtime |
| **Simulation** | CARLA, SUMO |
| **HD Maps** | Lanelet2 |
| **Observability** | OpenTelemetry, Prometheus, Grafana |
| **Testing** | Google Test, pytest |
| **CI/CD** | GitHub Actions |

## 📊 Quality Standards

- **Test Coverage**: ≥ 90% per component
- **Build Success**: 100%
- **Lint Pass Rate**: 100%
- **Documentation**: 100% public API
- **Security**: 0 critical/high findings

## 📜 License

Apache License 2.0 — See [LICENSE](LICENSE) for details.

---

<div align="center">

*Built with safety, modularity, and extensibility as core principles.*

</div>
