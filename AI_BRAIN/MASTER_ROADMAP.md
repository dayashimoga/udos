# Master Roadmap & Gap Analysis (AIPBF v4.0)

> **Generated**: 2026-06-02

---

## Roadmap

- **Phase 1**: Dynamic compilation & topological build validation. (Completed)
- **Phase 2**: Autonomous trajectory planning in CARLA simulation. (Completed)
- **Phase 3**: Hardware-in-the-loop track testing on physical platforms. (Planned)
- **Phase 4**: Production safety envelope compliance verification. (Planned)

---

## Gap Analysis

- **Missing Test Evidence**: No JUnit XML test logs verified on disk.  
- **Missing Coverage Evidence**: No Cobertura/coverage XML reports verified on disk.  


---

## Enhancement Opportunities

- Deconstruct file analyzer.py into smaller cohesive functional classes.
- Deconstruct file generator.py into smaller cohesive functional classes.
- Refactor module to remove unsafe API calls. Raw console printf instead of thread-safe logger
- Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared)
- Refactor module to remove unsafe API calls. Use of shell command execution (system)
- Refactor module to remove unsafe API calls. Use of shell pipe execution (popen)
- Refactor module to remove unsafe API calls. Use of unsafe buffer function (strcpy)


---

## Extension Points

| Target Component | Extension Directory | Expected Interfaces / base classes |
|:---|:---|:---|
| **New Sensor Driver** | `sensors/` or `hal/sensors/` | Inherit from `ISensor` interface. Add parsing for NMEA/lidar frames. |
| **New Motion Planner** | `planning/` | Inherit from `IPlanner`. Implement trajectory solver steps. |
| **New Lateral/Long Controller** | `control/` | Inherit from `IController`. Define yaw/speed output logic. |
| **New Safety Boundary Monitor** | `safety/` | Inherit from `ISafetyMonitor`. Define failsafe trigger conditions. |
| **New Fleet / Vehicle Driver** | `fleet/drivers/` or `fleet/` | Implement communication protocols for OTA rollbacks or fleet telemetry. |

---

## AI/ML Model Registry

| Model Name | Framework | Model File | Location | Source File | Verification |
|:---|:---|:---|:---|:---|:---|
| **Aipbf_export Model** | `ONNX Runtime` | `UNKNOWN` | `aipbf_export/` | `aipbf_export/analyzer.py` | VERIFIED |
| **Aipbf_export Model** | `TensorRT` | `UNKNOWN` | `aipbf_export/` | `aipbf_export/analyzer.py` | VERIFIED |
| **Aipbf_export Model** | `PyTorch/TorchScript` | `UNKNOWN` | `aipbf_export/` | `aipbf_export/analyzer.py` | VERIFIED |
| **Aipbf_export Model** | `TensorFlow` | `UNKNOWN` | `aipbf_export/` | `aipbf_export/analyzer.py` | VERIFIED |
| **Aipbf_export Model** | `OpenCV DNN` | `UNKNOWN` | `aipbf_export/` | `aipbf_export/analyzer.py` | VERIFIED |
