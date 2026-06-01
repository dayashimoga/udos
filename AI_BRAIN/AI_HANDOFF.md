# AI Handoff Document (AIPBF v4.0)

> **Generated**: 2026-06-01
> **Purpose**: Enable any AI model to restore full project context after context loss, model switching, or conversation reset.

---

## Current State
- **Build**: Presets configured.
- **Tests**: UNKNOWN GTest pass rate.
- **Deployment**: Operational presets.
- **Coverage**: UNKNOWN

## What Works (Implemented)
- Verified active directories: `/core`, `/hal`, `/sensors`, `/control`, `/safety`, `/fleet`, `/docs`, `/scripts`, `/prediction`, `/perception`, `/localization`, `/simulation`, `/validation`, `/.github`, `/aipbf_export`, `/AI_BRAIN`, `/configs`, `/digital_twin`, `/planning`.

## What Doesn't Work (Known Issues)
- Found 0 security vulnerabilities and 21 unsafe findings.

## Missing Work (Pending)
- Integrate JUnit XML export to verify testing pass rates.

## Highest Priority (Next Steps)
- Configure CMake presets, compile C++ targets, and execute test validation suites.

## Risks & Blockers
- None.

## If Continuing Development Start Here
- Setup environment and bootstrap dependencies.

---

## Context Restoration Payload

```json
{
  "project": "Autonomous Driving Operating System",
  "architecture": "Event Driven Decoupled Subsystems",
  "primary_flow": "Sensors -> Perception -> Localization -> Prediction -> Planning -> Control -> Safety -> HAL",
  "key_technologies": [
    "C++",
    "CMake",
    "Conan",
    "Eigen",
    "GTest",
    "Markdown",
    "ONNX Runtime",
    "OpenCV",
    "Python",
    "YAML",
    "gRPC"
  ],
  "implemented_capabilities": [
    "CAP-001 (Lane Detection)",
    "CAP-002 (Obstacle Detection)",
    "CAP-003 (Trajectory Planning)",
    "CAP-004 (Emergency Braking)",
    "CAP-005 (Vehicle Localization)",
    "CAP-006 (Sensor Fusion)",
    "CAP-007 (OTA Updates)",
    "CAP-008 (Digital Twin Simulation)"
  ],
  "pending_capabilities": [],
  "known_risks": [
    "Sensor calibration drift",
    "Localization divergence",
    "CAN bus timing drops"
  ],
  "next_priorities": [
    "Configure CMake presets, compile C++ targets, and execute test validation suites"
  ]
}
```

---

## Build & Run Commands

| Action | Command |
|:---|:---|
| **Setup** | `conan install . --build=missing` |
| **Compile** | `cmake --preset release` & `cmake --build --preset release` |
| **Test** | `ctest --output-on-failure` |
| **Run** | `./build/release/bin/test_uados_kernel` |

---

## AI Development Contract

Before modifying code:
1. **Read AIPBF**: Understand the fact-based repository architecture index.
2. **Read Requirements**: Check [MASTER_REQUIREMENTS.md](./MASTER_REQUIREMENTS.md) to preserve the functional criteria.
3. **Read ADRs**: Check decisions in [MASTER_DECISIONS.md](./MASTER_DECISIONS.md) to avoid replacing optimized controllers or algorithms.
4. **Read Architecture Rules**: Ensure your code changes do not bypass safety boundaries or violate layer isolation.

When implementing:
1. **Update tests**: Add unit tests, negative test scenarios, and edge boundaries.
2. **Update requirements traceability**: Annotate new code sections with explicit `REQ-` tags.
3. **Update documentation**: Document all public functions, classes, and architectural changes.
4. **Update capability registry**: Reflect any new or refactored capability mappings.

Before marking complete:
1. **Build passes**: Verify the code compiles without warnings.
2. **Tests pass**: Verify that all standard and edge-case unit tests pass.
3. **Coverage maintained**: Maintain or improve unit test coverage bounds.
4. **Documentation updated**: Run the Project Brain scanner to sync facts.

---

## Architecture Boundaries

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

---

## Document Cross-References

| Document | Purpose |
|:---|:---|
| [PROJECT_BRAIN.md](./PROJECT_BRAIN.md) | Master index |
| [MASTER_ARCHITECTURE.md](./MASTER_ARCHITECTURE.md) | Architecture details |
| [MASTER_REQUIREMENTS.md](./MASTER_REQUIREMENTS.md) | Requirements traceability |
| [MASTER_SECURITY.md](./MASTER_SECURITY.md) | Security audit |
| [MASTER_TESTING.md](./MASTER_TESTING.md) | Test evidence |
| [MASTER_KNOWLEDGE_GRAPH.md](./MASTER_KNOWLEDGE_GRAPH.md) | Domain models & messages |
| [MASTER_RISKS.md](./MASTER_RISKS.md) | Risk & failure modes |
| [MASTER_PROGRESS.md](./MASTER_PROGRESS.md) | Feature registry & readiness |
| [MASTER_VALIDATION_STATUS.md](./MASTER_VALIDATION_STATUS.md) | Architecture drift detection |
