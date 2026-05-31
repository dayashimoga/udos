# UADOS — Universal Autonomous Driving Operating System
# Operational Runbook & Production Deployment Guide

## 1. System Architecture Overview

UADOS is a layered autonomous driving operating system written in C++20 for high-performance safety-critical execution. Subsystems communicate via a zero-copy shared memory event bus, orchestrated by a Rate-Monotonic Scheduler.

```
+-----------------------------------------------------------------+
|                   FLEET TELEMETRY & OTA UPDATES                 |
+-----------------------------------------------------------------+
|                   VALIDATION COMPLIANCE SUITE                   |
+-----------------------------------------------------------------+
|                   SAFETY PLATFORM MONITOR & FSM                 |
+-----------------------------------------------------------------+
|   PERCEPTION  -->   LOCALIZATION  -->   PLANNING  -->  CONTROL  |
+-----------------------------------------------------------------+
|                       VEHICLE DRIVER (HAL)                      |
+-----------------------------------------------------------------+
|                        VEHICLE OS KERNEL                        |
+-----------------------------------------------------------------+
```

---

## 2. Startup and Initialization Sequence

To boot UADOS on vehicle computers (e.g. Orin, Workstation, sub-scale RC platform):

1. **Power Ingestion**: Verify vehicle bus battery supply is nominal ($12.0\text{V} - 14.2\text{V}$).
2. **Execute Setup Hooks**: Run the boot loader configuration script:
   ```bash
   ./scripts/setup/dev_env_setup.sh
   ```
3. **Launch UADOS Microkernel daemon**:
   ```bash
   ./build/core/kernel/uados_kernel_daemon --config configs/vehicle/carla_simulation.yaml
   ```
   *Expected Telemetry Output:*
   ```json
   {"timestamp":"2026-05-30T17:40:00Z","level":"INFO","component":"core.kernel","message":"UADOS Kernel Facade booted successfully."}
   ```

---

## 3. Cellular Telemetry Configuration

Local vehicle parameters package into ISO-formatted JSON payloads, transmitted over Cellular gRPC cells.

*Default Ingestion Packet format:*
```json
{
  "vehicle_id": "uados-ego-carla-001",
  "timestamp": "2026-05-30T17:40:00Z",
  "kinematics": {
    "x": 40.0,
    "y": -1.5,
    "vx": 12.5
  },
  "diagnostics": {
    "health": 0,
    "cross_track_error": 0.05,
    "heading_error": 0.01,
    "emergency_active": false
  }
}
```

*Configuring Cellular telemetry rate (Hz):*
Edit `configs/fleet/telemetry.yaml`:
```yaml
fleet:
  telemetry:
    cellular_rate_hz: 10.0
    gRPC_endpoint: "cloud.uados-autonomy.com:50051"
```

---

## 4. Diagnostics Error Codes Guide

UADOS actively audits system invariants. Below is the active diagnostics lookup catalog:

| Error Code | Level | Description | Safety Action | Recovery Strategy |
|---|---|---|---|---|
| **E-BOS-001** | `Warning` | Simultaneous throttle and brake commands requested. | Clamps throttle input to $0.0$ immediately, letting brake dominate. | Check actuator CAN calibration thresholds. |
| **E-SPD-002** | `Warning` | Ego velocity exceeds lanelet speed limit + buffer. | Logs diagnostic warning; alerts speed controller to decelerate. | Normal longitudinal PID tracking correction. |
| **E-ODD-003** | `Critical` | Cross-track lateral drift exceeds $1.8\text{m}$ boundary. | Triggers safety override envelope; forces `emergency_stop = true` with full stop brake pressure ($1.0$). | Transitions FSM to `SafeState`, shifts gear to `Park`, triggers cellular alert. |
| **E-OTA-004** | `Critical` | Over-the-air package checksum or SemVer validation failed. | Terminates deployment rollout; increments rollback counters. | Rollbacks active version immediately to stable `"0.1.0"`. |

---

## 5. Emergency FSM Recovery Procedures

When the **Emergency Response System (ERS)** triggers a **Minimum Risk Condition (MRC)** due to critical anomalies:

```
[Normal] --(Anomaly)--> [ActiveMRC: decel at -3.0 m/s²] --(Speed <= 0.1)--> [SafeState: Park gear lock]
```

To recover and resume nominal autonomous operation:
1. Ensure the dynamic hazard has cleared the safety tunnel envelope ($> 1.8\text{m}$).
2. Perform manual drive-by-wire override check or clear diagnostics:
   ```bash
   uados-cli diagnostics reset-safety-monitor
   ```
3. Command ERS to restore nominal status:
   ```bash
   uados-cli emergency reset-nominal
   ```
   *System will transition back to `Normal` state and resume strategic path routing.*

---

## 6. Over-The-Air (OTA) Updates & Rollbacks

Software hot-updates can be deployed dynamically across vehicle fleets:

### 6.1 OTA Package Verification
The `OTAManager` executes three check stages prior to rollout:
1. **SemVer Verification**: Version must be newer than the active package.
2. **Checksum Integrity Check**: Compares packet contents against DJB2 expected checksum signatures:
   ```cpp
   unsigned long hash = 5381;
   for (char c : payload) {
       hash = ((hash << 5) + hash) + c;
   }
   ```

### 6.2 Rollback Deployment
If checksum checking fails or dynamic shared library loading raises a fault, UADOS executes a **Safe Rollback**:
1. Aborts package extraction.
2. Restores nominal plugin settings.
3. Automatically rolls active version back to stable version `"0.1.0"`.
4. Increments rollback diagnostic metrics and alerts cellular operators.

---
*Operational Runbook verified and locked. Ready for production deployment.*
