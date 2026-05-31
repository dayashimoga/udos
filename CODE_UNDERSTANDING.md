# UADOS — Plain English System Guide

This document maps out the core subsystems of UADOS.

---

## 1. System Boot & Initialization
The kernel system maps configurations, creates event bus routing rings, and schedules recurring threads:
- **File**: `core/kernel/include/uados/kernel.hpp`
- **Class**: `uados::core::Kernel`

---

## 2. Lock-free Interprocess Communication
Decoupled modules communicate zero-copy via the circular IPC ring:
- **File**: `core/event_bus/include/uados/event_bus.hpp`
- **Class**: `uados::core::EventBus`

---

## 3. Estimated Path Planning & Tracking
Fuses coordinate tracks using a Kalman filter and executes lateral steering geometry:
- **Sensors**: `sensors/fusion/include/uados/sensor_fusion.hpp`
- **Control**: `control/steering/include/uados/stanley_controller.hpp`
