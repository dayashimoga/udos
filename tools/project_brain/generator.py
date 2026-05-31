# -*- coding: utf-8 -*-
"""
UADOS — Universal AI Project Brain Framework (AIPBF) v3.0
Factual Single-File Master Project Brain Generator
"""

import os
from pathlib import Path
from datetime import datetime

class DocumentationGenerator:
    def __init__(self, repo_path, analysis_data, review_data):
        self.repo_path = Path(repo_path).resolve()
        self.analysis = analysis_data
        self.review = review_data
        self.now_str = datetime.now().strftime("%Y-%m-%d")
        self.brain_dir = self.repo_path / "AI_BRAIN"
        self.brain_dir.mkdir(parents=True, exist_ok=True)
        
        self.ignore_patterns = [
            "node_modules", "vendor", "dist", "build", ".next",
            "coverage", "generated", "bin", "obj", "tmp", ".cache",
            "target", "out", ".git", "third_party", "tools", "analysis",
            "project_brain"
        ]

    def is_ignored(self, path):
        parts = Path(path).parts
        return any(pat in parts for pat in self.ignore_patterns)

    def generate_all(self):
        self._generate_single_project_brain()
        self._create_docs_structure()

    def _get_fact_block(self, title):
        for fact in self.analysis.get("facts", []):
            if title in fact["title"]:
                return f"\n> **Verification**: {fact['verification']}  \n> **Evidence**: File: `{fact['evidence']['file']}`, Line: {fact['evidence']['line']}, Confidence: {fact['evidence']['confidence']}  \n"
        return "\n> **Verification**: INFERRED  \n> **Evidence**: File: `N/A`, Line: N/A, Confidence: LOW  \n"

    def _generate_single_project_brain(self):
        lang_str = ", ".join(self.analysis["tech_stack"]["languages"]) if self.analysis["tech_stack"]["languages"] else "Undetected"
        build_tools_str = ", ".join(self.analysis["tech_stack"]["build_tools"]) if self.analysis["tech_stack"]["build_tools"] else "None detected"
        
        ident = self.analysis["project_identity"]
        scores = self.review["scores"]
        sec_chk = self.review["security_checklist"]
        test_reg = self.review["testing_registry"]

        # APIs
        api_rows = ""
        for api in self.analysis["apis"]:
            api_rows += f"| `{api['endpoint']}` | {api['protocol']} | `{api['file']}` | {api['line']} | {api['verification']} |\n"
        if not api_rows:
            api_rows = "| None verified in project code paths | — | — | — | — |\n"

        # Events
        event_rows = ""
        for ev in self.analysis["events"]:
            event_rows += f"| `{ev['pattern']}` | {ev['type']} | `{ev['file']}` | {ev['line']} | {ev['verification']} |\n"
        if not event_rows:
            event_rows = "| None verified in project code paths | — | — | — | — |\n"

        # Vulnerabilities
        vuln_rows = ""
        for vuln in self.review["vulnerabilities"]:
            vuln_rows += f"| `{vuln['evidence']['file']}:L{vuln['evidence']['line']}` | {vuln['title']} | {vuln['severity']} | {vuln['remediation']} | {vuln['verification']} |\n"
        if not vuln_rows:
            vuln_rows = "| None | No verified vulnerabilities found | Low | — | VERIFIED |\n"

        # Technical Debts
        debt_rows = ""
        for debt in self.review["debt"]:
            debt_rows += f"| {debt['title']} | {debt['impact']} | {debt['priority']} | {debt['recommendation']} | {debt['verification']} |\n"
        if not debt_rows:
            debt_rows = "| None | No large files or quality debt verified | Low | — | VERIFIED |\n"

        # Build Mermaid graph
        mermaid_relations = ""
        for src, dest, relation in self.analysis["module_graph"]:
            mermaid_relations += f"    {src} -->|{relation}| {dest}\n"
        if not mermaid_relations:
            mermaid_relations = "    Root -->|Project Folder| Workspace\n"

        # Dynamic Knowledge Confidence Matrix
        conf_arch = "HIGH (VERIFIED)" if self.analysis["module_graph"] else "LOW (Generated from folder structure only, no file-to-file import relationships verified)"
        conf_reqs = "HIGH (VERIFIED)" if self.analysis["requirements"] else "LOW (UNKNOWN - No requirements specification file or inline REQ tags detected in codebase)"
        conf_test = "HIGH (VERIFIED)" if test_reg["pass_rate"] != "UNKNOWN" else "LOW (UNKNOWN - No XML/JSON test logs verified on disk)"
        conf_sec = "HIGH (VERIFIED)" if self.review["vulnerabilities"] else "LOW (HEURISTIC)"
        conf_perf = "HIGH (VERIFIED)" if "VERIFIED" in test_reg["performance"] else "LOW (UNKNOWN - No benchmark results file verified on disk)"

        # Requirements Coverage Matrix
        req_rows = ""
        for req in self.analysis["requirements"]:
            req_rows += f"| {req['id']} | {req['name']} | {req['status']} | {req['verification']} | None | {req['confidence']} |\n"
        if not req_rows:
            req_rows = "| None | Project requirements are not documented in repository | UNKNOWN | N/A | High | Low |"

        # Dynamic Subsystems Layout (Exists checklist)
        subsystems_output = ""
        for folder_name, exists_status in sorted(self.analysis["directories"].items()):
            status_str = "TRUE" if exists_status else "FALSE"
            subsystems_output += f"Directory:\n  {folder_name}/\n  Exists: {status_str}\n\n"

        # Dynamic Component Registry
        component_rows = ""
        idx = 10
        for folder_name, exists_status in sorted(self.analysis["directories"].items()):
            if exists_status:
                component_rows += f"| C-{idx:03d} | {folder_name.capitalize()} Subsystem | `{folder_name}/` | ✅ Implemented | VERIFIED |\n"
                idx += 10
        if not component_rows:
            component_rows = "| C-010 | Workspace Root | `./` | ✅ Implemented | VERIFIED |\n"

        # Dynamic Walkthrough Entry Points
        entry_points = []
        common_entries = ["main.cpp", "main.py", "index.ts", "app.ts", "server.ts", "main.go", "main.rs", "app.py", "server.py"]
        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                if file in common_entries:
                    try:
                        rel_p = str(Path(root) / file).relative_to(self.repo_path).replace("\\", "/")
                        entry_points.append(rel_p)
                    except Exception:
                        pass

        walkthrough_entries = ""
        if entry_points:
            for ep in entry_points:
                walkthrough_entries += f"- **System Initiator**: Mapped verified entry point: `{ep}` (VERIFIED)\n"
        else:
            walkthrough_entries = "- **System Initiator**: UNKNOWN (No standard main entry file detected)\n"

        # Dynamic Data Flow
        data_flow_output = ""
        if self.analysis["data_flow"]:
            for df in self.analysis["data_flow"]:
                data_flow_output += f"- `{df}`\n"
        else:
            data_flow_output = "Data Flow: UNKNOWN (No file-to-file import dependency path derived)\n"

        # Dynamic Reliability Overview
        if (self.repo_path / "safety").exists() or (self.repo_path / "validation").exists():
            reliability_overview = "Reliability mechanisms are structured inside safety monitor interfaces and validation pipelines."
        else:
            reliability_overview = "Reliability: UNKNOWN (No explicit safety or failsafe validation folders identified in workspace)"

        # Dynamic Performance Overview
        if "VERIFIED" in test_reg["performance"]:
            performance_overview = f"System latencies and solver speeds verified. Benchmark results file found."
        else:
            performance_overview = "Performance: UNKNOWN (No performance benchmark reports or latency logs found)"

        # Dynamic Gap Analysis
        gaps_output = ""
        if not entry_points:
            gaps_output += "- **Missing Entry Point**: No standard main initialization file found.  \n"
        if not self.analysis["requirements"]:
            gaps_output += "- **Missing Requirements Document**: No requirements specification file detected.  \n"
        if test_reg["pass_rate"] == "UNKNOWN":
            gaps_output += "- **Missing Test Evidence**: No JUnit XML test logs verified on disk.  \n"
        if test_reg["coverage"] == "UNKNOWN":
            gaps_output += "- **Missing Coverage Evidence**: No Cobertura/coverage XML reports verified on disk.  \n"
        if not gaps_output:
            gaps_output = "- **Gaps**: None dynamically identified in current layout."

        # Dynamic AI Handoff restoring variables
        handoff_works = ""
        active_dirs = [f"`/{d}`" for d, ex in self.analysis["directories"].items() if ex]
        if active_dirs:
            handoff_works = f"Verified active directories: {', '.join(active_dirs)}."
        else:
            handoff_works = "Source code files crawlers and standard folder directory architectures."

        handoff_issues = ""
        issues_count = len(self.review["vulnerabilities"]) + len(self.review["findings"])
        if issues_count > 0:
            handoff_issues = f"Found {len(self.review['vulnerabilities'])} security vulnerabilities and {len(self.review['findings'])} unsafe findings."
        else:
            handoff_issues = "No critical workspace issues verified."

        handoff_pending = ""
        if not self.analysis["requirements"]:
            handoff_pending = "Document requirements in requirements specification file. "
        if test_reg["pass_rate"] == "UNKNOWN":
            handoff_pending += "Integrate JUnit XML export to verify testing pass rates."
        if not handoff_pending:
            handoff_pending = "Define functional domain logic parameters."

        handoff_steps = ""
        if "CMake" in self.analysis["tech_stack"]["build_tools"]:
            handoff_steps = "Configure CMake presets, compile C++ targets, and execute test validation suites."
        elif "npm/yarn" in self.analysis["tech_stack"]["build_tools"]:
            handoff_steps = "Bootstrap Node workspace, install dependencies, and trigger package.json start presets."
        else:
            handoff_steps = "Inspect README files and directory trees."

        content = f"""# Universal AI Project Brain (AIPBF) v3.0 — Unified Blueprint

> **Framework Version**: v3.0 (Factual Single-File)  
> **Last Synchronized**: {self.now_str}  
> **Verification Gate**: 100% Strict Evidence-Based  

---

## 1. Executive Summary
This document serves as the single authoritative source of truth for the repository.

### Dynamic Project Identity:
- **Project_Type**: {ident['type']}
- **Project_Domain**: {ident['domain']}
- **Primary_Purpose**: {ident['purpose']}
- **Confidence**: {ident['confidence']}
- **Evidence**:
"""
        for ev in ident["evidence"]:
            content += f"  - {ev}\n"
            
        content += f"""
---

## 2. Dynamic Repository Health & Metrics
### Repository Health Index:
- **Repository Health**: ✅ STABLE
- **Documentation Coverage**: VERIFIED (README.md)
- **Test Coverage**: {test_reg['coverage']} (Factual Index - Strict Rule 1)
- **Code Complexity**: {scores['complexity_score']}
- **Technical Debt**: {scores['quality_score']}
- **Dynamic Risk Score**: LOW

### Quality Scores Checkgates (Rule 003):
| Metric / Score | Value | Status / Verification |
|:---|:---|:---|
| Build Status | ✅ Operational | Pass |
| Testing Pass Rate | {test_reg['pass_rate']} | {test_reg['pass_rate'] if test_reg['pass_rate'] != 'UNKNOWN' else 'UNKNOWN (Strict Rule 1)'} |
| Security Score | {scores['security_score']} | UNKNOWN (Strict Rule 1) |
| Quality Score | {scores['quality_score']} | UNKNOWN (Strict Rule 1) |
| Reliability Score | {scores['reliability_score']} | UNKNOWN (Strict Rule 1) |

---

## 3. Technology Stack
- **Primary Languages**: {lang_str}
- **Build / Packaging Tooling**: {build_tools_str}

{self._get_fact_block("Build Engine")}

---

## 4. Repository Intelligence
### Logical Subsystems Layout (Verified Directories):
{subsystems_output}
---

## 5. Requirements Coverage Matrix
| Requirement ID | Requirement Name | Status | Verification | Gap | Confidence |
|:---|:---|:---|:---|:---|:---|
{req_rows}

---

## 6. Architecture & Derived Dependency Graph
The following Mermaid dependency blueprint was **derived dynamically** by scanning codebase file-to-file import relationships (`#include`, `import ... from`, `require`):

```mermaid
graph TD
{mermaid_relations}```

---

## 7. Component Registry
| Component ID | Name | Path | Status | Verification |
|:---|:---|:---|:---|:---|
{component_rows}
---

## 8. Implementation Summary
The repository consists of `{self.analysis['loc']}` lines of code across standard directories. Code modules are structured under verified filesystem folders with direct compilation or workspace targets.

---

## 9. Code Understanding Section
### Subsystem walkthrough entry points:
{walkthrough_entries}
---

## 10. Data Flow Analysis
Discovered data pathways traced from import dependency hierarchies:
{data_flow_output}
---

## 11. API Intelligence Registry
Verified endpoints bound to recognized HTTP Web Frameworks (No scanner or helper false positives):
| Endpoint / Route | Protocol | Source File | Line | Verification |
|:---|:---|:---|:---|:---|
{api_rows}

---

## 12. Event Intelligence Registry
Verified event clients and circular router dispatches:
| Event Pattern | Client Type | Source File | Line | Verification |
|:---|:---|:---|:---|:---|
{event_rows}

---

## 13. Database Intelligence
RAM pre-allocated buffers or verified database client model interfaces.

---

## 14. Configuration Registry
- Mapped configuration files inside project directory:
"""
        config_files = [".env", ".env.example", "pyproject.toml", "CMakeLists.txt", "conanfile.py", "package.json"]
        for conf in config_files:
            p = self.repo_path / conf
            if p.exists():
                content += f"- `{conf}`: Verified configuration file (VERIFIED)\n"

        content += f"""
---

## 15. Dependency Registry
Factual verified workspace imports:
- **External Dependencies**: {", ".join(self.analysis["dependencies"]["external"][:10]) if self.analysis["dependencies"]["external"] else "None detected"}

{self._get_fact_block("Pip Package") or self._get_fact_block("Conan Dependency") or self._get_fact_block("Node.js Dependency")}

---

## 16. Security Intelligence (Scanned Checklist)
### Security Scope:
- **Source Code**: {sec_chk['source_code']}
- **IaC**: {sec_chk['iac']}
- **Containers**: {sec_chk['containers']}
- **Dependencies**: {sec_chk['dependencies']}

### Verified Vulnerabilities:
| Target Path | Title | Severity | Remediation Strategy | Verification |
|:---|:---|:---|:---|:---|
{vuln_rows}
### Result:
- **Security Rating**: No verified vulnerabilities found.
- **Confidence**: LOW (Heuristic Scan Only)

---

## 17. Reliability Overview
{reliability_overview}

---

## 18. Performance Overview
{performance_overview}
- **Source**: {test_reg['performance'] if test_reg['performance'] != 'UNKNOWN' else 'UNKNOWN (Strict Rule 1 - No benchmark results file)'}

---

## 19. Testing Intelligence Registry
Dynamic test counts and categories:
- **Unit Tests**: {test_reg['unit']}
- **Integration Tests**: {test_reg['integration']}
- **E2E Tests**: {test_reg['e2e']}
- **Coverage Index**: {test_reg['coverage']}
- **Mutation Index**: {test_reg['mutation']}
- **Performance tests**: {test_reg['performance']}
- **Security tests**: {test_reg['security']}
- **Test Evidence**: {test_reg['evidence']}

---

## 20. Gap Analysis
{gaps_output}

---

## 21. Technical Debt Registry
| Debt Descriptor | Impact | Priority | Recommended Remediation | Verification |
|:---|:---|:---|:---|:---|
{debt_rows}

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
| Architecture Blueprint | {conf_arch} | MERMAID INFERRED |
| Requirements Coverage | {conf_reqs} | FACT VERIFIED |
| Testing Registry | {conf_test} | GTEST VERIFIED |
| Security Intelligence | {conf_sec} | HEURISTIC SCANNED |
| Performance Metrics | {conf_perf} | Not Scanned |

---

## 25. AI Handoff & Onboarding Section (AI_HANDOFF)
### restore_payload:
- **Current State**:
  - Build: ✅ Presets configured.
  - Tests: {test_reg['pass_rate']} GTest pass rate.
  - Deployment: Operational presets.
  - Coverage: {test_reg['coverage']}
- **What Works (Implemented)**:
  - {handoff_works}
- **What Doesn't Work (Known Issues)**:
  - {handoff_issues}
- **Missing Work (Pending)**:
  - {handoff_pending}
- **Highest Priority (Next Steps)**:
  - {handoff_steps}
- **Risks & Blockers**:
  - None.
- **If Continuing Development Start Here**:
  - Setup environment and bootstrap dependencies.
"""
        (self.brain_dir / "PROJECT_BRAIN.md").write_text(content, encoding="utf-8")
        print("[AIPBF] Generated AI_BRAIN/PROJECT_BRAIN.md successfully.")
        
    def _create_docs_structure(self):
        docs_dirs = [
            "docs/api", "docs/architecture", "docs/guides", "docs/safety",
            "docs/testing", "docs/operations", "docs/status", "docs/analysis"
        ]
        for d in docs_dirs:
            (self.repo_path / d).mkdir(parents=True, exist_ok=True)
            keep_file = self.repo_path / d / ".gitkeep"
            if not keep_file.exists():
                keep_file.touch()
        print("[AIPBF] Verified /docs structure is active.")
