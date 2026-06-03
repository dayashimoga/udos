# Master Component Index (AIPBF v4.0)

> **Generated**: 2026-06-03
> **Components**: 19

---

## Directory Verification

| Directory | Exists |
|:---|:---|
| `.github/` | TRUE |
| `AI_BRAIN/` | TRUE |
| `aipbf_export/` | TRUE |
| `analytics/` | FALSE |
| `backend/` | FALSE |
| `configs/` | TRUE |
| `control/` | TRUE |
| `core/` | TRUE |
| `database/` | FALSE |
| `digital_twin/` | TRUE |
| `docs/` | TRUE |
| `fleet/` | TRUE |
| `frontend/` | FALSE |
| `hal/` | TRUE |
| `infra/` | FALSE |
| `localization/` | TRUE |
| `perception/` | TRUE |
| `planning/` | TRUE |
| `prediction/` | TRUE |
| `safety/` | TRUE |
| `scripts/` | TRUE |
| `sensors/` | TRUE |
| `shared/` | FALSE |
| `simulation/` | TRUE |
| `tests/` | FALSE |
| `validation/` | TRUE |


---

## Component Registry

| Component ID | Name | Path | Status | Verification |
|:---|:---|:---|:---|:---|
| C-010 | .github Subsystem | `.github/` | Implemented | VERIFIED |
| C-020 | Ai_brain Subsystem | `AI_BRAIN/` | Implemented | VERIFIED |
| C-030 | Aipbf_export Subsystem | `aipbf_export/` | Implemented | VERIFIED |
| C-040 | Configs Subsystem | `configs/` | Implemented | VERIFIED |
| C-050 | Control Subsystem | `control/` | Implemented | VERIFIED |
| C-060 | Core Subsystem | `core/` | Implemented | VERIFIED |
| C-070 | Digital_twin Subsystem | `digital_twin/` | Implemented | VERIFIED |
| C-080 | Docs Subsystem | `docs/` | Implemented | VERIFIED |
| C-090 | Fleet Subsystem | `fleet/` | Implemented | VERIFIED |
| C-100 | Hal Subsystem | `hal/` | Implemented | VERIFIED |
| C-110 | Localization Subsystem | `localization/` | Implemented | VERIFIED |
| C-120 | Perception Subsystem | `perception/` | Implemented | VERIFIED |
| C-130 | Planning Subsystem | `planning/` | Implemented | VERIFIED |
| C-140 | Prediction Subsystem | `prediction/` | Implemented | VERIFIED |
| C-150 | Safety Subsystem | `safety/` | Implemented | VERIFIED |
| C-160 | Scripts Subsystem | `scripts/` | Implemented | VERIFIED |
| C-170 | Sensors Subsystem | `sensors/` | Implemented | VERIFIED |
| C-180 | Simulation Subsystem | `simulation/` | Implemented | VERIFIED |
| C-190 | Validation Subsystem | `validation/` | Implemented | VERIFIED |


---

## OWNERSHIP Matrix

| Subsystem Component | Target Subsystem Path | Owner Team / Responsibility | Verification |
|:---|:---|:---|:---|
| **Planning** | `planning/*` | Motion Planning Team | VERIFIED |
| **Safety** | `safety/*` | Safety Systems Team | VERIFIED |
| **Localization** | `localization/*` | State Estimation Team | VERIFIED |
| **Perception** | `perception/*` | Sensor Perception Team | VERIFIED |
| **Control** | `control/*` | Vehicle Controls Team | VERIFIED |
| **Sensors** | `sensors/*` | Hardware HAL Team | VERIFIED |
| **Core** | `core/*` | Platform Systems Team | VERIFIED |
| **HAL** | `hal/*` | Hardware HAL Team | VERIFIED |
| **Digital Twin** | `digital_twin/*` | Simulation Systems Team | VERIFIED |
| **Simulation** | `simulation/*` | Simulation Systems Team | VERIFIED |
| **Validation** | `validation/*` | Compliance Systems Team | VERIFIED |
| **Fleet** | `fleet/*` | Fleet Operations Team | VERIFIED |

---

## File Distribution

| Subsystem Module | Count of Scanned Files | Verification |
|:---|:---|:---|
| **Aipbf_export** | 4 source files | VERIFIED |
| **Control** | 6 source files | VERIFIED |
| **Core** | 23 source files | VERIFIED |
| **Digital_twin** | 4 source files | VERIFIED |
| **Fleet** | 4 source files | VERIFIED |
| **Hal** | 11 source files | VERIFIED |
| **Localization** | 6 source files | VERIFIED |
| **Perception** | 10 source files | VERIFIED |
| **Planning** | 6 source files | VERIFIED |
| **Prediction** | 6 source files | VERIFIED |
| **Safety** | 4 source files | VERIFIED |
| **Sensors** | 13 source files | VERIFIED |
| **Simulation** | 4 source files | VERIFIED |
| **Validation** | 4 source files | VERIFIED |


---

## AI Safe Modification Tiers

| Tier Level | Mapped Subsystems | Actionable AI Guidelines |
|:---|:---|:---|
| **Tier 1 (LOW RISK)** | `/docs`, `/simulation`, `/validation`, `/.github` | AI agents can safely modify, add test suites, compile scenarios, or optimize documentation. |
| **Tier 2 (MEDIUM RISK)** | `/control`, `/prediction`, `/perception`, `/localization`, `/planning` | Functional logic changes. Ensure to run localized validation suites and EKF accuracy tests. |
| **Tier 3 (HIGH RISK)** | `/core`, `/hal`, `/safety` | Real-time scheduling, safety monitors, or IPC layers. Modifying these requires architect approval. |
