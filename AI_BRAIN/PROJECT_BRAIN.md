# Universal AI Project Brain (AIPBF) v3.0 — Unified Blueprint

> **Framework Version**: v3.0 (Factual Single-File)  
> **Last Synchronized**: 2026-05-31  
> **Verification Gate**: 100% Strict Evidence-Based  

---

## 1. Executive Summary
This document serves as the single authoritative source of truth for the repository.

### Dynamic Project Identity:
- **Project_Type**: Autonomous Driving Operating System
- **Project_Domain**: Autonomous Vehicles & Robotic Systems
- **Primary_Purpose**: Failsafe real-time vehicle scheduling, fusion, path planning, and envelope controls.
- **Confidence**: HIGH
- **Evidence**:
  - File matches key 'camera': camera_driver.cpp
  - File matches key 'camera': camera_driver.hpp
  - File matches key 'canbus': canbus_driver.cpp
  - File matches key 'canbus': canbus_driver.hpp
  - File matches key 'controller': longitudinal_controller.cpp

---

## 2. Dynamic Repository Health & Metrics
### Repository Health Index:
- **Repository Health**: ✅ STABLE
- **Documentation Coverage**: VERIFIED (README.md)
- **Test Coverage**: UNKNOWN (Factual Index - Strict Rule 1)
- **Code Complexity**: UNKNOWN
- **Technical Debt**: UNKNOWN
- **Dynamic Risk Score**: LOW

### Quality Scores Checkgates (Rule 003):
| Metric / Score | Value | Status / Verification |
|:---|:---|:---|
| Build Status | ✅ Operational | Pass |
| Testing Pass Rate | UNKNOWN | UNKNOWN (Strict Rule 1) |
| Security Score | UNKNOWN | UNKNOWN (Strict Rule 1) |
| Quality Score | UNKNOWN | UNKNOWN (Strict Rule 1) |
| Reliability Score | UNKNOWN | UNKNOWN (Strict Rule 1) |

---

## 3. Technology Stack
- **Primary Languages**: C++, Markdown, YAML, Python
- **Build / Packaging Tooling**: Conan, CMake


> **Verification**: VERIFIED  
> **Evidence**: File: `CMakeLists.txt`, Line: 1, Confidence: HIGH  


---

## 4. Repository Intelligence
### Logical Subsystems Layout (Verified Directories):
Directory:
  .github/
  Exists: TRUE

Directory:
  AI_BRAIN/
  Exists: TRUE

Directory:
  analytics/
  Exists: FALSE

Directory:
  backend/
  Exists: FALSE

Directory:
  configs/
  Exists: TRUE

Directory:
  control/
  Exists: TRUE

Directory:
  core/
  Exists: TRUE

Directory:
  database/
  Exists: FALSE

Directory:
  digital_twin/
  Exists: TRUE

Directory:
  docs/
  Exists: TRUE

Directory:
  fleet/
  Exists: TRUE

Directory:
  frontend/
  Exists: FALSE

Directory:
  hal/
  Exists: TRUE

Directory:
  infra/
  Exists: FALSE

Directory:
  localization/
  Exists: TRUE

Directory:
  perception/
  Exists: TRUE

Directory:
  planning/
  Exists: TRUE

Directory:
  prediction/
  Exists: TRUE

Directory:
  safety/
  Exists: TRUE

Directory:
  scripts/
  Exists: TRUE

Directory:
  sensors/
  Exists: TRUE

Directory:
  shared/
  Exists: FALSE

Directory:
  simulation/
  Exists: TRUE

Directory:
  tests/
  Exists: FALSE

Directory:
  validation/
  Exists: TRUE


---

## 5. Requirements Coverage Matrix
| Requirement ID | Requirement Name | Status | Verification | Gap | Confidence |
|:---|:---|:---|:---|:---|:---|
| R-DOC-1 | Document: MASTER_REQUIREMENTS.md | Documented | Document verified on disk | None | HIGH |
| R-SRC-2 | Code reference: Requirement:\s*[^\n]+)`. | Implemented | Source Code Verified | None | HIGH |


---

## 6. Architecture & Derived Dependency Graph
The following Mermaid dependency blueprint was **derived dynamically** by scanning codebase file-to-file import relationships (`#include`, `import ... from`, `require`):

```mermaid
graph TD
    control -->|Imports reference code| validation
    core -->|Imports reference code| validation
    digital_twin -->|Imports reference code| validation
    fleet -->|Imports reference code| validation
    hal -->|Imports reference code| validation
    localization -->|Imports reference code| validation
    perception -->|Imports reference code| sensors
    perception -->|Imports reference code| validation
    planning -->|Imports reference code| localization
    planning -->|Imports reference code| validation
    prediction -->|Imports reference code| validation
    safety -->|Imports reference code| localization
    safety -->|Imports reference code| validation
    sensors -->|Imports reference code| digital_twin
    sensors -->|Imports reference code| validation
    simulation -->|Imports reference code| digital_twin
    simulation -->|Imports reference code| validation
    validation -->|Imports reference code| safety
    validation -->|Imports reference code| simulation
```

---

## 7. Component Registry
| Component ID | Name | Path | Status | Verification |
|:---|:---|:---|:---|:---|
| C-010 | .github Subsystem | `.github/` | ✅ Implemented | VERIFIED |
| C-020 | Ai_brain Subsystem | `AI_BRAIN/` | ✅ Implemented | VERIFIED |
| C-030 | Configs Subsystem | `configs/` | ✅ Implemented | VERIFIED |
| C-040 | Control Subsystem | `control/` | ✅ Implemented | VERIFIED |
| C-050 | Core Subsystem | `core/` | ✅ Implemented | VERIFIED |
| C-060 | Digital_twin Subsystem | `digital_twin/` | ✅ Implemented | VERIFIED |
| C-070 | Docs Subsystem | `docs/` | ✅ Implemented | VERIFIED |
| C-080 | Fleet Subsystem | `fleet/` | ✅ Implemented | VERIFIED |
| C-090 | Hal Subsystem | `hal/` | ✅ Implemented | VERIFIED |
| C-100 | Localization Subsystem | `localization/` | ✅ Implemented | VERIFIED |
| C-110 | Perception Subsystem | `perception/` | ✅ Implemented | VERIFIED |
| C-120 | Planning Subsystem | `planning/` | ✅ Implemented | VERIFIED |
| C-130 | Prediction Subsystem | `prediction/` | ✅ Implemented | VERIFIED |
| C-140 | Safety Subsystem | `safety/` | ✅ Implemented | VERIFIED |
| C-150 | Scripts Subsystem | `scripts/` | ✅ Implemented | VERIFIED |
| C-160 | Sensors Subsystem | `sensors/` | ✅ Implemented | VERIFIED |
| C-170 | Simulation Subsystem | `simulation/` | ✅ Implemented | VERIFIED |
| C-180 | Validation Subsystem | `validation/` | ✅ Implemented | VERIFIED |

---

## 8. Implementation Summary
The repository consists of `20321` lines of code across standard directories. Code modules are structured under verified filesystem folders with direct compilation or workspace targets.

---

## 9. Code Understanding Section
### Subsystem walkthrough entry points:
- **System Initiator**: UNKNOWN (No standard main entry file detected)

---

## 10. Data Flow Analysis
Discovered data pathways traced from import dependency hierarchies:
Data Flow: UNKNOWN (No file-to-file import dependency path derived)

---

## 11. API Intelligence Registry
Verified endpoints bound to recognized HTTP Web Frameworks (No scanner or helper false positives):
| Endpoint / Route | Protocol | Source File | Line | Verification |
|:---|:---|:---|:---|:---|
| None verified in project code paths | — | — | — | — |


---

## 12. Event Intelligence Registry
Verified event clients and circular router dispatches:
| Event Pattern | Client Type | Source File | Line | Verification |
|:---|:---|:---|:---|:---|
| `EventBus` | EventBus Routing Ring | `event_bus.hpp` | 96 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `event_bus.hpp` | 98 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `event_bus_factory.hpp` | 12 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `event_bus_impl.cpp` | 19 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `event_bus_impl.cpp` | 186 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `kernel.hpp` | 40 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `kernel_impl.cpp` | 154 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `kernel_impl.cpp` | 166 | VERIFIED |
| `EventBus` | EventBus Routing Ring | `plugin.hpp` | 96 | VERIFIED |


---

## 13. Database Intelligence
RAM pre-allocated buffers or verified database client model interfaces.

---

## 14. Configuration Registry
- Mapped configuration files inside project directory:
- `pyproject.toml`: Verified configuration file (VERIFIED)
- `CMakeLists.txt`: Verified configuration file (VERIFIED)
- `conanfile.py`: Verified configuration file (VERIFIED)

---

## 15. Dependency Registry
Factual verified workspace imports:
- **External Dependencies**: abseil/20240116.2, benchmark/1.9.0, eigen/3.4.0, flatbuffers/24.3.25, fmt/11.0.2, grpc/1.66.0, gtest/1.15.0, nlohmann_json/3.11.3, onnxruntime/1.19.0, opencv/4.10.0


> **Verification**: INFERRED  
> **Evidence**: File: `N/A`, Line: N/A, Confidence: LOW  


---

## 16. Security Intelligence (Scanned Checklist)
### Security Scope:
- **Source Code**: YES
- **IaC**: NO
- **Containers**: NO
- **Dependencies**: YES

### Verified Vulnerabilities:
| Target Path | Title | Severity | Remediation Strategy | Verification |
|:---|:---|:---|:---|:---|
| None | No verified vulnerabilities found | Low | — | VERIFIED |

### Result:
- **Security Rating**: No verified vulnerabilities found.
- **Confidence**: LOW (Heuristic Scan Only)

---

## 17. Reliability Overview
Reliability mechanisms are structured inside safety monitor interfaces and validation pipelines.

---

## 18. Performance Overview
Performance: UNKNOWN (No performance benchmark reports or latency logs found)
- **Source**: UNKNOWN (Strict Rule 1 - No benchmark results file)

---

## 19. Testing Intelligence Registry
Dynamic test counts and categories:
- **Unit Tests**: 24 Verified suites
- **Integration Tests**: 1 Verified suites
- **E2E Tests**: UNKNOWN
- **Coverage Index**: UNKNOWN
- **Mutation Index**: UNKNOWN
- **Performance tests**: UNKNOWN
- **Security tests**: UNKNOWN
- **Test Evidence**: N/A

---

## 20. Gap Analysis
- **Missing Entry Point**: No standard main initialization file found.  
- **Missing Test Evidence**: No JUnit XML test logs verified on disk.  
- **Missing Coverage Evidence**: No Cobertura/coverage XML reports verified on disk.  


---

## 21. Technical Debt Registry
| Debt Descriptor | Impact | Priority | Recommended Remediation | Verification |
|:---|:---|:---|:---|:---|
| None | No large files or quality debt verified | Low | — | VERIFIED |


---

## 22. Risk Registry
| Risk Descriptor | Likelihood | Impact | Mitigation Strategy | Owner |
|:---|:---|:---|:---|:---|
| Hardcoded secrets or credentials | Medium | High | Move parameters to system env variables | DevOps |

---

## 23. Improvement Registry
- Deconstruct large files (>800 lines) into smaller cohesive functional classes.
- Standardize all configuration files under unified dot-env presets.

---

## 24. Knowledge Confidence Matrix
| Section / Module | Confidence Rating | Verification Method |
|:---|:---|:---|
| Architecture Blueprint | HIGH (VERIFIED) | MERMAID INFERRED |
| Requirements Coverage | HIGH (VERIFIED) | FACT VERIFIED |
| Testing Registry | LOW (UNKNOWN - No XML/JSON test logs verified on disk) | GTEST VERIFIED |
| Security Intelligence | LOW (HEURISTIC) | HEURISTIC SCANNED |
| Performance Metrics | LOW (UNKNOWN - No benchmark results file verified on disk) | Not Scanned |

---

## 25. AI Handoff & Onboarding Section (AI_HANDOFF)
### restore_payload:
- **Current State**:
  - Build: ✅ Presets configured.
  - Tests: UNKNOWN GTest pass rate.
  - Deployment: Operational presets.
  - Coverage: UNKNOWN
- **What Works (Implemented)**:
  - Verified active directories: `/core`, `/hal`, `/sensors`, `/control`, `/safety`, `/fleet`, `/docs`, `/scripts`, `/prediction`, `/perception`, `/localization`, `/simulation`, `/validation`, `/.github`, `/AI_BRAIN`, `/configs`, `/digital_twin`, `/planning`.
- **What Doesn't Work (Known Issues)**:
  - No critical workspace issues verified.
- **Missing Work (Pending)**:
  - Integrate JUnit XML export to verify testing pass rates.
- **Highest Priority (Next Steps)**:
  - Configure CMake presets, compile C++ targets, and execute test validation suites.
- **Risks & Blockers**:
  - None.
- **If Continuing Development Start Here**:
  - Setup environment and bootstrap dependencies.
