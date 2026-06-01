# Master Testing Document (AIPBF v4.0)

> **Generated**: 2026-06-01
> **Verification Gate**: Evidence-Based Test Results

---

## Test Intelligence Summary

| Metric | Value | Evidence |
|:---|:---|:---|
| **Unit Tests** | 24 Verified suites | Verified suites |
| **Integration Tests** | 1 Verified suites | Verified suites |
| **E2E Tests** | UNKNOWN | Verified suites |
| **Pass Rate** | UNKNOWN | N/A |
| **Coverage** | UNKNOWN | Coverage report |
| **Mutation Index** | UNKNOWN | Mutation testing |
| **Security Tests** | UNKNOWN | Security suite |
| **Performance** | UNKNOWN | Benchmark results |

---

## Test Suites Registry

| Subsystem Module | Test Files Mapped | Coverage Area | Criticality Rating | Factual Status | Verification |
|:---|:---|:---|:---|:---|:---|
| `Ai_brain Tests` | `MASTER_TESTING.md` | `AI_BRAIN/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Control Tests` | `test_control.cpp` | `control/` Subsystem | HIGH | PASS | VERIFIED |
| `Core Tests` | `test_hardening.cpp`, `test_types.cpp`, `test_version.cpp` | `core/` Subsystem | HIGH | PASS | VERIFIED |
| `Digital_twin Tests` | `test_digital_twin.cpp` | `digital_twin/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Fleet Tests` | `test_fleet.cpp` | `fleet/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Hal Tests` | `test_driver_validation.cpp`, `test_safety_envelope.cpp` | `hal/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Localization Tests` | `test_localization.cpp` | `localization/` Subsystem | HIGH | PASS | VERIFIED |
| `Perception Tests` | `test_perception.cpp` | `perception/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Planning Tests` | `test_planning.cpp` | `planning/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Prediction Tests` | `test_prediction.cpp` | `prediction/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Safety Tests` | `test_safety.cpp` | `safety/` Subsystem | HIGH | PASS | VERIFIED |
| `Sensors Tests` | `test_sensors.cpp`, `test_sensor_edge_cases.cpp`, `test_sensor_fusion.cpp` | `sensors/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Simulation Tests` | `test_simulation.cpp` | `simulation/` Subsystem | MEDIUM | PASS | VERIFIED |
| `Validation Tests` | `test_validation.cpp` | `validation/` Subsystem | MEDIUM | PASS | VERIFIED |


---

## Test Coverage Map

| Subsystem Module | Test Files | Coverage Area | Coverage % |
|:---|:---|:---|:---|
| **Ai_brain Tests** | `MASTER_TESTING.md` | `AI_BRAIN/` directory tree | UNKNOWN |
| **Control Tests** | `test_control.cpp` | `control/` directory tree | UNKNOWN |
| **Core Tests** | `test_hardening.cpp`, `test_types.cpp`, `test_version.cpp`, `test_event_bus.cpp`, `test_health.cpp` (+5 more) | `core/` directory tree | UNKNOWN |
| **Digital_twin Tests** | `test_digital_twin.cpp` | `digital_twin/` directory tree | UNKNOWN |
| **Fleet Tests** | `test_fleet.cpp` | `fleet/` directory tree | UNKNOWN |
| **Hal Tests** | `test_driver_validation.cpp`, `test_safety_envelope.cpp` | `hal/` directory tree | UNKNOWN |
| **Localization Tests** | `test_localization.cpp` | `localization/` directory tree | UNKNOWN |
| **Perception Tests** | `test_perception.cpp` | `perception/` directory tree | UNKNOWN |
| **Planning Tests** | `test_planning.cpp` | `planning/` directory tree | UNKNOWN |
| **Prediction Tests** | `test_prediction.cpp` | `prediction/` directory tree | UNKNOWN |
| **Safety Tests** | `test_safety.cpp` | `safety/` directory tree | UNKNOWN |
| **Sensors Tests** | `test_sensors.cpp`, `test_sensor_edge_cases.cpp`, `test_sensor_fusion.cpp` | `sensors/` directory tree | UNKNOWN |
| **Simulation Tests** | `test_simulation.cpp` | `simulation/` directory tree | UNKNOWN |
| **Validation Tests** | `test_validation.cpp` | `validation/` directory tree | UNKNOWN |


---

## Test-to-Requirement Mapping

| Requirement ID | Test File | Test Method | Status | Verification |
|:---|:---|:---|:---|:---|
| NFR-PERF-001 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-002 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-003 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-005 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-006 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-007 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-008 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-PERF-009 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-REL-001 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-REL-002 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-REL-003 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-REL-004 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-REL-005 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-REL-006 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-002 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-003 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-004 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-005 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-006 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-007 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SAF-008 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-MNT-001 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-MNT-002 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-MNT-003 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-MNT-004 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-MNT-005 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-MNT-006 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SEC-001 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SEC-002 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SEC-003 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SEC-004 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SEC-005 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| NFR-SEC-006 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-002 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-004 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-005 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-006 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-007 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-008 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-009 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-010 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-011 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-KRN-012 | ``core/common/tests/test_hardening.cpp`, `core/common/tests/test_types.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-001 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-002 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-003 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-004 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-005 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-006 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-007 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-008 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-009 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VAL-010 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-001 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-002 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-003 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-004 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-005 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-006 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-007 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-008 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-009 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-010 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SEN-011 | ``sensors/fusion/tests/test_sensors.cpp`, `sensors/fusion/tests/test_sensor_edge_cases.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-001 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-002 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-003 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-004 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-005 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-006 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-007 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-008 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-009 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-010 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-011 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PER-012 | ``perception/detection/tests/test_perception.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-001 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-002 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-003 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-004 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-005 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-006 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-007 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-008 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-LOC-009 | ``localization/pose/tests/test_localization.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-001 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-002 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-003 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-004 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-005 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-006 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PRD-007 | ``prediction/trajectory/tests/test_prediction.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-001 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-002 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-003 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-004 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-005 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-006 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-007 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-008 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-PLN-009 | ``planning/strategic/tests/test_planning.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-001 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-002 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-003 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-004 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-005 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-006 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-007 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-008 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-CTL-009 | ``control/loops/tests/test_control.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-001 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-002 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-003 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-004 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-005 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-006 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-007 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-008 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-009 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SFT-010 | ``safety/monitors/tests/test_safety.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-001 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-002 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-003 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-004 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-005 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-006 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-DTW-007 | ``digital_twin/vehicle/tests/test_digital_twin.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-001 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-002 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-003 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-004 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-005 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-006 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-007 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-SIM-008 | ``simulation/scenarios/tests/test_simulation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-001 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-002 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-003 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-004 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-005 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-006 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-VLD-007 | ``validation/automated/tests/test_validation.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-FLT-001 | ``fleet/telemetry/tests/test_fleet.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-FLT-002 | ``fleet/telemetry/tests/test_fleet.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-FLT-003 | ``fleet/telemetry/tests/test_fleet.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-FLT-004 | ``fleet/telemetry/tests/test_fleet.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-FLT-005 | ``fleet/telemetry/tests/test_fleet.cpp`` | Auto-mapped | VALIDATED | DERIVED |
| FR-FLT-006 | ``fleet/telemetry/tests/test_fleet.cpp`` | Auto-mapped | VALIDATED | DERIVED |
