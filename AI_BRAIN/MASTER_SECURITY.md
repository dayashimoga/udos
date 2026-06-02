# Master Security Document (AIPBF v4.0)

> **Generated**: 2026-06-02
> **Vulnerabilities Found**: 0
> **Unsafe Findings**: 21
> **Verification Gate**: SAST Heuristic Scan

---

## Security Posture Summary

| Security Dimension | Status |
|:---|:---|
| **Source Code Scan** | YES |
| **IaC Scan** | NO |
| **Container Scan** | YES |
| **Dependency Scan** | YES |

---

## Vulnerability Registry

| File Location | Vulnerability | Severity | Remediation | Verification |
|:---|:---|:---|:---|:---|
| None | No verified vulnerabilities found | Low | N/A | VERIFIED |


---

## Secrets & Credentials Scan

| File Location | Vulnerability Category | Impact | Remediation Strategy |
|:---|:---|:---|:---|
| None | No hardcoded credentials detected in codebase | None | N/A |


---

## Memory Safety Scan

| File Location | Unsafe Allocation Method | Impact | Remediation Strategy |
|:---|:---|:---|:---|
| `aipbf_export/generator.py:L652` | `Use of unsafe buffer function (strcpy)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Use of unsafe buffer function (strcpy) |
| `aipbf_export/generator.py:L662` | `Use of unsafe buffer function (strcpy)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Use of unsafe buffer function (strcpy) |
| `aipbf_export/generator.py:L330` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `aipbf_export/generator.py:L332` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `aipbf_export/generator.py:L652` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `aipbf_export/reviewer.py:L84` | `Use of unsafe buffer function (strcpy)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Use of unsafe buffer function (strcpy) |
| `aipbf_export/reviewer.py:L87` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/event_bus/include/uados/event_bus/event_bus_factory.hpp:L11` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/health/include/uados/health/health_monitor.hpp:L103` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/kernel/include/uados/kernel/config_manager.hpp:L38` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/kernel/include/uados/kernel/kernel.hpp:L48` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/kernel/include/uados/kernel/memory_pool.hpp:L45` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/lifecycle/include/uados/lifecycle/lifecycle_manager.hpp:L85` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/plugin/include/uados/plugin/plugin.hpp:L147` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/plugin/include/uados/plugin/plugin.hpp:L159` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `core/scheduler/include/uados/scheduler/scheduler.hpp:L115` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `perception/detection/tests/test_perception.cpp:L92` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |
| `perception/tracking/src/object_tracker.cpp:L116` | `Raw pointer new allocation (recommend std::make_unique or std::make_shared)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Raw pointer new allocation (recommend std::make_unique or std::make_shared) |


---

## Shell Execution Scan

| File Location | Shell Command Call | Impact | Remediation Strategy |
|:---|:---|:---|:---|
| `aipbf_export/generator.py:L664` | `Use of shell command execution (system)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Use of shell command execution (system) |
| `aipbf_export/generator.py:L664` | `Use of shell pipe execution (popen)` | Potential memory safety violation, buffer overflow, or arbitrary code execution. | Refactor module to remove unsafe API calls. Use of shell pipe execution (popen) |


---

## Unsafe Deserialization Scan

| File Location | Parser Signature Matching | Impact | Remediation Strategy |
|:---|:---|:---|:---|
| None | No unsafe deserialization parsing patterns detected | None | N/A |


---

## Technical Debt (Security-Related)

| Debt Descriptor | Impact | Priority | Recommended Remediation | Verification |
|:---|:---|:---|:---|:---|
| Large Source File Complexity | Increased dynamic cognitive load and difficult refactoring | Medium | Deconstruct file analyzer.py into smaller cohesive functional classes. | VERIFIED |
| Large Source File Complexity | Increased dynamic cognitive load and difficult refactoring | Medium | Deconstruct file generator.py into smaller cohesive functional classes. | VERIFIED |
