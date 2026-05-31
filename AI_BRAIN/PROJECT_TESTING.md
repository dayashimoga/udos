# AIPBF v2.0 — Dynamic Testing Registry

> **Verification**: VERIFIED  
> **Confidence**: HIGH  

---

## 1. C++ Google Test Inventory
- **Kernel Core Test Suite**: `core/kernel/tests/test_kernel.cpp` (VERIFIED)
- **Event Bus IPC Test Suite**: `core/event_bus/tests/test_event_bus.cpp` (VERIFIED)
- **Sensor Fusion Test Suite**: `sensors/fusion/tests/test_sensor_fusion.cpp` (VERIFIED)
- **Stanley Controller Test Suite**: `control/loops/tests/test_control.cpp` (VERIFIED)
- **Safety Envelope Test Suite**: `safety/monitors/tests/test_safety.cpp` (VERIFIED)

---

## 2. Rigorous Coverage Status (Rule 1)
- **Dynamic Unit Coverage**: UNKNOWN (Strict Rule 1 - Factual index requires external `.xml` log coverage imports)
- **Mutation testing**: UNKNOWN
- **E2E Integration**: VERIFIED in simulation visual twin dashboard.
