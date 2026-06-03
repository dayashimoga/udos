# Master Risks Document (AIPBF v4.0)

> **Generated**: 2026-06-03
> **Risk Categories**: Domain Risks, Failure Modes, State Machine Transitions

---

## Project Domain Risks

| Risk Descriptor | Likelihood | Impact | Mitigation Strategy | Owner |
|:---|:---|:---|:---|:---|
| Sensor calibration drift | Low | High | Automated EKF covariance checks & bounds | Fusion |
| Localization divergence | Low | High | Fallback map-relative position checkpoints | Localizer |
| CAN bus timing drops | Medium | High | Hardware rate throttling limits & safety overrides | Platform |
| Model inference latency spikes | Low | High | TensorRT pre-allocations & deadline watchdogs | Perception |
| Preemptive watchdog starvation | Low | Critical | Scheduler deadline partitions & high thread priorities | SRE |
| Failsafe OTA rollback failure | Low | Critical | Independent bootloader partition switch | DevOps |

---

## FAILURE_MODES (FMEA)

| Failure Mode | Detected By | Root Cause | System Effect | Failsafe Action / Mitigation |
|:---|:---|:---|:---|:---|
| **Sensor Drift (IMU/GPS)** | EKF Covariance boundary check | Hardware thermal drift | Inaccurate vehicle localization | Degrade to odometry only, decelerate |
| **Control Loop Lag (100Hz)**| Lifecycle Watchdog timer | Thread scheduling deadlock | Steer/velocity command loss | Trigger emergency hardware brake stop |
| **CAN Bus Dropped Frame** | Driver Timeout checking | Bus load congestion | Actuator feedback lost | Preempt with safety monitor, hold state |
| **LiDAR Obstacle Miss** | Perception Kalman validation | Extreme rainfall / occlusion | Late obstacle path planning | Engage conservative velocity limits |
| **Power Supply Voltage Drop**| HAL ADC voltage monitor | Actuator load spike | Incomplete steer engagement | Engage hardware battery redundancy channel |

---

## STATE_MACHINE_REGISTRY

| Current State | Event Trigger | Next State | Subsystem Action | Impact / Risk |
|:---|:---|:---|:---|:---|
| **BOOT** | Power On / Reset | **INIT** | Initialize Kernel, LifecycleManager & memory pools | Low |
| **INIT** | All subsystems registered | **READY** | Self-test passes, EKF converges, actuators check | Low |
| **READY** | Drive command received | **DRIVING** | Active control loop engagement (100Hz) | Medium |
| **DRIVING** | Obstacle inside emergency envelope | **EMERGENCY** | SafetyMonitor preempts control, deceleration | Critical |
| **DRIVING** | Minor sensor loss / glitch | **RECOVERY** | Switch to redundancy channel, rate limit | High |
| **EMERGENCY** | Safe vehicle state reached (MRC) | **RECOVERY** | Engage safe-harbor, pull-over check | Medium |
| **RECOVERY** | Diagnostic checklist clear | **READY** | Reset failsafes, verify CAN bus state | Low |
| **DRIVING / READY** | Power off command | **SHUTDOWN** | Flush buffers, close event brokers, shutdown HAL | Low |

---

## Change Impact Analysis

| Subsystem | Dependents (If Changed) | Risk Level | Impact Description |
|:---|:---|:---|:---|
| `core` | `aipbf_export` | High | Modifying `core` impacts compilation of 1 subsystems. Run regression validation. |
| `digital_twin` | `sensors`, `simulation` | High | Modifying `digital_twin` impacts compilation of 2 subsystems. Run regression validation. |
| `localization` | `planning`, `safety` | High | Modifying `localization` impacts compilation of 2 subsystems. Run regression validation. |
| `safety` | `validation` | High | Modifying `safety` impacts compilation of 1 subsystems. Run regression validation. |
| `sensors` | `perception` | High | Modifying `sensors` impacts compilation of 1 subsystems. Run regression validation. |
| `simulation` | `validation` | High | Modifying `simulation` impacts compilation of 1 subsystems. Run regression validation. |
| `validation` | `control`, `core`, `digital_twin`, `fleet`, `hal`, `localization`, `perception`, `planning`, `prediction`, `safety`, `sensors`, `simulation` | High | Modifying `validation` impacts compilation of 12 subsystems. Run regression validation. |
