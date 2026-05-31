# UADOS — Master Risk Registry

> **Version**: 0.1.0  
> **Status**: Draft  
> **Last Updated**: 2026-05-30  
> **Owner**: UADOS Architecture Team

---

## Risk Scoring

| Probability | Score | Definition |
|------------|-------|------------|
| Very Low | 1 | < 10% chance |
| Low | 2 | 10–30% |
| Medium | 3 | 30–60% |
| High | 4 | 60–85% |
| Very High | 5 | > 85% |

| Impact | Score | Definition |
|--------|-------|------------|
| Negligible | 1 | Minor inconvenience, no schedule impact |
| Low | 2 | Workaround exists, < 1 week delay |
| Medium | 3 | Significant rework, 1–4 week delay |
| High | 4 | Major redesign, 1–3 month delay |
| Critical | 5 | Project-threatening, safety-critical |

**Risk Score** = Probability × Impact

---

## 1. Technical Risks

| ID | Risk | Prob | Impact | Score | Mitigation | Owner | Status |
|----|------|------|--------|-------|------------|-------|--------|
| TR-001 | Zero-copy event bus introduces shared memory corruption | 2 | 5 | 10 | Extensive testing, ASAN/MSAN in CI, read-only consumer access, reference counting | Kernel Team | Open |
| TR-002 | CARLA simulation fidelity gap causes false confidence | 3 | 4 | 12 | RC car validation before production; document known gaps; fidelity metrics | Sim Team | Open |
| TR-003 | C++ ABI compatibility issues with plugins | 3 | 3 | 9 | C interface at plugin boundary; version checks; CI tests with multiple compilers | Kernel Team | Open |
| TR-004 | ONNX Runtime performance insufficient on target hardware | 2 | 3 | 6 | TensorRT fallback; model optimization; benchmark in CI | Perception Team | Open |
| TR-005 | Real-time scheduling not achievable on Linux | 2 | 4 | 8 | Use PREEMPT_RT kernel; CPU isolation; benchmark latency in CI | Kernel Team | Open |
| TR-006 | Sensor fusion (EKF) divergence under extreme conditions | 3 | 4 | 12 | Divergence detection; automatic reset; fallback to single-source; extensive sim testing | Sensor Team | Open |
| TR-007 | HD Map (Lanelet2) missing features for our use cases | 2 | 3 | 6 | Map abstraction layer; custom extensions; contribute upstream | Localization Team | Open |
| TR-008 | ML model training requires more data than available | 3 | 3 | 9 | Synthetic data from CARLA; transfer learning from public datasets; data augmentation | Perception Team | Open |
| TR-009 | Memory leaks in long-running processes | 3 | 4 | 12 | ASAN/LSAN in CI; periodic leak detection tests; memory pool design | All Teams | Open |
| TR-010 | Build times become prohibitive as codebase grows | 4 | 2 | 8 | Incremental builds; ccache; precompiled headers; module boundaries; remote cache | Build Team | Open |

## 2. Safety Risks

| ID | Risk | Prob | Impact | Score | Mitigation | Owner | Status |
|----|------|------|--------|-------|------------|-------|--------|
| SR-001 | Safety monitor fails to detect critical fault | 1 | 5 | 5 | Dual-channel monitoring; independent implementation; fault injection testing ≥ 99% | Safety Team | Open |
| SR-002 | Emergency braking latency exceeds 50ms | 2 | 5 | 10 | Dedicated CPU core for safety; pre-computed braking trajectories; hardware watchdog | Safety Team | Open |
| SR-003 | Perception fails to detect pedestrian in edge case | 3 | 5 | 15 | Multi-sensor redundancy; adversarial scenario testing; conservative safety envelope | Perception Team | Open |
| SR-004 | Control command sent to wrong actuator | 1 | 5 | 5 | Command plausibility checking; actuator address verification; hardware interlocks | Control Team | Open |
| SR-005 | System continues operating outside ODD boundary | 2 | 5 | 10 | ODD monitor with conservative thresholds; automatic MRC transition | Safety Team | Open |
| SR-006 | RC car loses communication with compute platform | 3 | 3 | 9 | Hardware failsafe (ESC neutral on signal loss); watchdog on RC receiver; tethered mode | RC Team | Open |
| SR-007 | Software update introduces regression in safety-critical path | 2 | 5 | 10 | Regression test suite; phased rollout; rollback capability; safety test gate in CI | Safety Team | Open |

## 3. Project Risks

| ID | Risk | Prob | Impact | Score | Mitigation | Owner | Status |
|----|------|------|--------|-------|------------|-------|--------|
| PR-001 | Context window limitations lose architectural knowledge | 4 | 3 | 12 | AI_BRAIN context preservation framework; delta updates only; master documents | AI | Open |
| PR-002 | Scope creep from additional vehicle platform requirements | 3 | 3 | 9 | Strict phase gating; driver abstraction isolates platform-specific work | Product Owner | Open |
| PR-003 | CARLA dependency introduces breaking changes | 2 | 3 | 6 | Pin CARLA version; abstraction layer isolates CARLA specifics | Sim Team | Open |
| PR-004 | Third-party library abandoned or deprecated | 2 | 3 | 6 | Abstraction layers; multiple alternatives identified; license audit | Arch Team | Open |
| PR-005 | Integration complexity across 15 phases | 4 | 3 | 12 | Continuous integration from Phase 2; integration tests in every phase | All Teams | Open |
| PR-006 | RC car hardware procurement delays | 3 | 2 | 6 | Order hardware early; simulation-first approach reduces hardware dependency | RC Team | Open |

## 4. Security Risks

| ID | Risk | Prob | Impact | Score | Mitigation | Owner | Status |
|----|------|------|--------|-------|------------|-------|--------|
| SEC-001 | CAN bus injection attack on production vehicle | 2 | 5 | 10 | CAN message authentication (MAC); gateway filtering; intrusion detection | Security Team | Open |
| SEC-002 | OTA update tampered with in transit | 1 | 5 | 5 | Code signing; TLS 1.3; certificate pinning; staged rollout | Security Team | Open |
| SEC-003 | Shared memory accessible by unauthorized process | 2 | 4 | 8 | POSIX permissions; process namespace isolation; capability-based access | Kernel Team | Open |
| SEC-004 | Telemetry data exfiltration | 2 | 3 | 6 | Encryption at rest and in transit; data minimization; access controls | Fleet Team | Open |
| SEC-005 | Supply chain attack via compromised dependency | 2 | 4 | 8 | Pin dependency versions; verify checksums; Conan lockfiles; security scanning | Build Team | Open |

---

## 5. Preliminary Hazard Analysis (HARA)

| Hazard ID | Hazard | Severity | Probability | ASIL | Mitigation |
|-----------|--------|----------|-------------|------|------------|
| H-001 | Unintended acceleration | S3 | E3 | ASIL-C | Throttle plausibility check; independent brake authority; safety envelope |
| H-002 | Loss of steering control | S3 | E3 | ASIL-C | Steering command validation; rate limiting; fallback straight-line trajectory |
| H-003 | Failure to detect obstacle | S3 | E4 | ASIL-D | Multi-sensor redundancy; conservative safety distance; emergency braking |
| H-004 | Incorrect localization leading to wrong lane | S3 | E3 | ASIL-C | Multi-source localization; map matching validation; lane departure detection |
| H-005 | Planning generates infeasible trajectory | S2 | E3 | ASIL-B | Kinematic feasibility check; fallback trajectory; trajectory tracking monitor |
| H-006 | Loss of all sensors | S3 | E2 | ASIL-B | Graceful degradation; immediate MRC transition; pre-computed safe stop |
| H-007 | Software crash during autonomous driving | S3 | E3 | ASIL-C | Watchdog; automatic restart; independent safety process; hardware failsafe |
| H-008 | Communication loss (RC car/fleet) | S2 | E4 | ASIL-B | Hardware failsafe (neutral); local autonomy continues; safe stop on timeout |
| H-009 | Incorrect prediction leads to collision | S3 | E3 | ASIL-C | Conservative gap maintenance; safety envelope; emergency braking available |
| H-010 | Map data outdated or incorrect | S2 | E3 | ASIL-B | Perception-based validation of map data; degraded mode without map |

---

## 6. Risk Heat Map

```
Impact →     1           2           3           4           5
Prob ↓   Negligible     Low        Medium       High      Critical
  5      |           |           |           |           |          |
  4      |           | TR-010    | PR-001    |           |          |
         |           |           | PR-005    |           |          |
  3      |           |           | TR-003    | TR-002    | SR-003   |
         |           | PR-006    | TR-008    | TR-006    |          |
         |           |           | SR-006    | TR-009    |          |
  2      |           |           | TR-004    | TR-005    | SR-002   |
         |           |           | TR-007    | SEC-003   | SR-005   |
         |           |           | PR-003    | SEC-005   | SEC-001  |
         |           |           | PR-004    |           | SR-007   |
  1      |           |           |           |           | SR-001   |
         |           |           |           |           | SR-004   |
         |           |           |           |           | SEC-002  |
```

**Legend**: Red zone (score ≥ 12) requires immediate mitigation plan. Orange zone (8-11) needs active monitoring. Green zone (< 8) acceptable risk.

---

## 7. Technical Debt Tracker

| ID | Debt Item | Introduced | Impact | Plan to Address | Phase |
|----|-----------|-----------|--------|----------------|-------|
| TD-001 | (Reserved for Phase 1+) | — | — | — | — |

> Technical debt items will be added as implementation progresses. This section is initialized empty.

---

*End of Master Risk Registry*
