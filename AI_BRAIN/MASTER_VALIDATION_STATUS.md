# Master Validation Status (AIPBF v4.0)

> **Generated**: 2026-06-02
> **Purpose**: Change Impact Engine output, Architecture Drift Detection, Validation Rules

---

## Architecture Drift Detection

| Subsystem | Declared Dependencies | Actual Dependencies | Status |
|:---|:---|:---|:---|
| `control` | common, eventbus | validation | DRIFT (undeclared: validation) |
| `core` | common, eventbus | validation | DRIFT (undeclared: validation) |
| `localization` | common, eventbus | validation | DRIFT (undeclared: validation) |
| `perception` | common, eventbus, sensors | sensors, validation | DRIFT (undeclared: validation) |
| `planning` | common, eventbus, localization, prediction | localization, validation | DRIFT (undeclared: validation) |
| `prediction` | common, eventbus, perception | validation | DRIFT (undeclared: validation) |
| `safety` | common, eventbus, localization | localization, validation | DRIFT (undeclared: validation) |
| `sensors` | common, digital_twin, eventbus | digital_twin, validation | DRIFT (undeclared: validation) |


---

## Forward Impact Analysis

If module X changes, these downstream modules are affected:

| Module Changed | Affected Downstream | Impact Description |
|:---|:---|:---|
| `aipbf_export` | `core` | 1 downstream modules affected |
| `control` | `validation` | 1 downstream modules affected |
| `core` | `validation` | 1 downstream modules affected |
| `digital_twin` | `validation` | 1 downstream modules affected |
| `fleet` | `validation` | 1 downstream modules affected |
| `hal` | `validation` | 1 downstream modules affected |
| `localization` | `validation` | 1 downstream modules affected |
| `perception` | `sensors`, `validation` | 2 downstream modules affected |
| `planning` | `localization`, `validation` | 2 downstream modules affected |
| `prediction` | `validation` | 1 downstream modules affected |
| `safety` | `localization`, `validation` | 2 downstream modules affected |
| `sensors` | `digital_twin`, `validation` | 2 downstream modules affected |
| `simulation` | `digital_twin`, `validation` | 2 downstream modules affected |
| `validation` | `safety`, `simulation` | 2 downstream modules affected |


---

## Tier Boundary Violations

| Source Module | Target Module | Violated Rule | Severity |
|:---|:---|:---|:---|
| `aipbf_export` | `core` | Tier 1 (aipbf_export) depends on Tier 3 critical module (core) | HIGH |
| `validation` | `safety` | Tier 1 (validation) depends on Tier 3 critical module (safety) | HIGH |


---

## Knowledge Confidence Matrix

| Section / Module | Confidence Rating | Verification Method |
|:---|:---|:---|
| Architecture Blueprint | MEDIUM (DERIVED) | MERMAID DERIVED |
| Requirements Coverage | HIGH (VERIFIED) | FACT VERIFIED |
| Testing Registry | LOW (UNKNOWN) | GTEST VERIFIED |
| Security Intelligence | LOW (HEURISTIC) | HEURISTIC SCANNED |
| Performance Metrics | LOW (UNKNOWN) | Not Scanned |
| Domain Models | HIGH (VERIFIED) | STRUCT SCAN |
| Message Catalog | LOW (No pub/sub patterns found) | PATTERN SCAN |
| Boot Flow | HIGH (VERIFIED) | ENTRY SCAN |
| AI/ML Models | HIGH (VERIFIED) | FRAMEWORK SCAN |

---

## CONFIGURATION_SCHEMA

| Configuration File | Type | Secrets Detected | Verification |
|:---|:---|:---|:---|
| `.github/workflows/ci.yml` | YAML | No | VERIFIED |
| `.github/workflows/docs-sync.yml` | YAML | No | VERIFIED |
| `configs/vehicle/carla_simulation.yaml` | YAML | No | VERIFIED |
| `configs/vehicle/rc_car.yaml` | YAML | No | VERIFIED |
| `pyproject.toml` | TOML | No | VERIFIED |

### Configuration Parameters Schema
| Config Parameter | Type | Default Value | Validation Rule | Subsystem Impact |
|:---|:---|:---|:---|:---|
| `control.steering.p_gain` | Float | `0.85` | `0.1 <= P <= 3.0` | Stanley steering lateral controller loops |
| `control.speed.max_velocity` | Float | `15.0 m/s` | `V_MAX <= 25.0` | Longitudinal PID velocity controller limits |
| `localization.ekf.noise_covariance` | FloatArray | `[0.01, 0.01]` | Non-zero diagonal elements | EKF sensor fusion convergence bounds |
| `safety.envelope.margin_seconds` | Float | `1.5s` | `0.8 <= margin <= 3.0` | Time-to-collision safety override envelope |
| `sensors.camera.frame_rate` | Integer | `30` | `10 <= fps <= 60` | Camera acquisition and perception pipe inputs |

---

## PERFORMANCE_BUDGETS

| Subsystem Layer | Latency Budget | CPU Core Limit | Memory Pool Allocation | ASIL Target |
|:---|:---|:---|:---|:---|
| **Core Kernel / EventBus** | <= 1ms | Core 0 (Dedicated) | 16 MB (Static lockless) | ASIL-D |
| **Sensors & Driver HAL** | <= 5ms | Core 1 | 32 MB (Static ring buffer)| ASIL-B |
| **Localization (EKF)** | <= 10ms | Core 2 | 64 MB | ASIL-B |
| **Perception (LiDAR/Cam)**| <= 50ms | Core 3 (GPU bound) | 256 MB (TensorRT) | ASIL-B |
| **Planning & Behaviors** | <= 20ms | Core 4 | 128 MB | ASIL-B |
| **Control Loop (Stanley)** | <= 5ms | Core 5 | 8 MB | ASIL-C |
| **Safety Envelope Monitor**| <= 2ms | Core 0 (Dedicated) | 4 MB (Isolated memory) | ASIL-D |

---

## Validation Rules

AI must never:
- Delete Architecture
- Modify Public Contracts
- Remove Tests
- Remove Security Controls
- Modify Database Schema

without updating:
- Requirements
- Architecture
- Tests
- Deployment
