# -*- coding: utf-8 -*-
"""
UADOS — Universal AI Project Brain Framework (AIPBF) v3.1
Factual Reviewer Engine & Quality Metrics Gatekeeper
"""

import os
import re
import xml.etree.ElementTree as ET
from pathlib import Path

class RepositoryReviewer:
    def __init__(self, repo_path, analyzer_data):
        self.repo_path = Path(repo_path).resolve()
        self.analyzer = analyzer_data
        self.findings = []
        self.vulnerabilities = []
        self.debt_items = []
        
        # Rigorous v3.1 Security Checklist
        self.security_checklist = {
            "source_code": "YES",
            "iac": "NO",
            "containers": "NO",
            "dependencies": "NO"
        }

        # Rigorous v3.1 Test Registry
        self.testing_registry = {
            "unit": "UNKNOWN",
            "integration": "UNKNOWN",
            "e2e": "UNKNOWN",
            "coverage": "UNKNOWN",
            "mutation": "UNKNOWN",
            "performance": "UNKNOWN",
            "security": "UNKNOWN",
            "pass_rate": "UNKNOWN",
            "evidence": "N/A"
        }
        
        self.metrics = {
            "security_score": "UNKNOWN",
            "quality_score": "UNKNOWN",
            "reliability_score": "UNKNOWN",
            "complexity_score": "UNKNOWN"
        }
        
        self.ignore_patterns = [
            "node_modules", "vendor", "dist", "build", ".next",
            "coverage", "generated", "bin", "obj", "tmp", ".cache",
            "target", "out", ".git", "third_party", "tools", "analysis",
            "project_brain"
        ]

    def is_ignored(self, path):
        parts = Path(path).parts
        return any(pat in parts for pat in self.ignore_patterns)

    def review(self):
        self._audit_codebase()
        self._evaluate_testing_evidence()
        self._verify_operational_benchmarks()
        self._calculate_factual_scores()
        return {
            "scores": self.metrics,
            "security_checklist": self.security_checklist,
            "testing_registry": self.testing_registry,
            "findings": self.findings,
            "vulnerabilities": self.vulnerabilities,
            "debt": self.debt_items
        }

    def _audit_codebase(self):
        secret_patterns = [
            (r'(?i)(?:key|secret|password|passwd|token|credential|keyfile)\s*=\s*["\'][a-zA-Z0-9_\-\.\/]{8,}["\']', "Hardcoded Secrets"),
            (r'(?i)private_key\s*=\s*["\']-+BEGIN', "Private Key disclosure")
        ]

        # Rigid v3.1 C++ & Dynamic Script safety scanners (Fix 7)
        unsafe_patterns = [
            (r'\beval\([^)]*\)', "Use of dynamic code execution (eval)", "High", "Security"),
            (r'\bsystem\([^)]*\)', "Use of shell command execution (system)", "High", "Security"),
            (r'\bpopen\([^)]*\)', "Use of shell pipe execution (popen)", "Medium", "Security"),
            (r'\bstrcpy\b', "Use of unsafe buffer function (strcpy)", "Medium", "Reliability"),
            (r'\bprintf\b', "Raw console printf instead of thread-safe logger", "Low", "Quality"),
            (r'\bmalloc\([^)]*\)', "Raw malloc buffer allocation (recommend std::vector or unique_ptr)", "Low", "Quality"),
            (r'\bnew\s+\w+', "Raw pointer new allocation (recommend std::make_unique or std::make_shared)", "Low", "Quality"),
            (r'unserialize\(|JSON\.parse\(', "Potential unsafe deserialization parser", "Low", "Security")
        ]

        for root, dirs, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            
            for file in files:
                file_name = file.lower()
                if file_name.endswith((".tf", ".tfvars")) or "terraform" in file_name:
                    self.security_checklist["iac"] = "YES"
                if "dockerfile" in file_name or "docker-compose" in file_name:
                    self.security_checklist["containers"] = "YES"
                if file_name in ["package.json", "conanfile.py", "requirements.txt", "go.mod", "cargo.toml"]:
                    self.security_checklist["dependencies"] = "YES"

            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".py", ".go", ".rs", ".cs", ".js", ".ts", ".java"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        lines = content.splitlines()
                        relative_path = str(file_path.relative_to(self.repo_path)).replace("\\", "/")

                        if len(lines) > 800:
                            self.debt_items.append({
                                "title": "Large Source File Complexity",
                                "impact": "Increased dynamic cognitive load and difficult refactoring",
                                "priority": "Medium",
                                "effort": "1-2 Developer days",
                                "files": [relative_path],
                                "recommendation": f"Deconstruct file {file_path.name} into smaller cohesive functional classes.",
                                "verification": "VERIFIED",
                                "evidence": {
                                    "file": relative_path,
                                    "line": 1,
                                    "confidence": "HIGH"
                                }
                            })

                        for pat, title in secret_patterns:
                            matches = re.finditer(pat, content)
                            for match in matches:
                                line_num = content[:match.start()].count("\n") + 1
                                self.vulnerabilities.append({
                                    "title": title,
                                    "severity": "CRITICAL",
                                    "description": "Hardcoded security secret parameter discovered in plain-text module source.",
                                    "evidence": {
                                        "file": relative_path,
                                        "line": line_num,
                                        "confidence": "HIGH"
                                    },
                                    "impact": "Exploitable by repository accessors to compromise cloud backends.",
                                    "likelihood": "High",
                                    "exploitability": "High",
                                    "remediation": "Move credentials to system environment variables or gitignored configuration files.",
                                    "verification": "VERIFIED"
                                })

                        for pat, desc, severity, category in unsafe_patterns:
                            matches = re.finditer(pat, content)
                            for match in matches:
                                line_num = content[:match.start()].count("\n") + 1
                                self.findings.append({
                                    "title": f"Unsafe Implementation: {desc}",
                                    "severity": severity,
                                    "description": desc,
                                    "evidence": {
                                        "file": relative_path,
                                        "line": line_num,
                                        "confidence": "HIGH"
                                    },
                                    "category": category,
                                    "impact": "Potential memory safety violation, buffer overflow, or arbitrary code execution.",
                                    "likelihood": "Medium",
                                    "exploitability": "Medium",
                                    "remediation": f"Refactor module to remove unsafe API calls. {desc}",
                                    "verification": "VERIFIED"
                                })

                    except Exception:
                        pass

    def _evaluate_testing_evidence(self):
        unit_cnt = 0
        integration_cnt = 0
        e2e_cnt = 0

        for root, dirs, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                file_name = file.lower()
                if "test_" in file_name or "spec" in file_name or "_test" in file_name:
                    file_path = Path(root) / file
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        if "gtest" in content or "pytest" in content or "unittest" in content or "jest" in content:
                            if "integration" in file_name or "integration" in content.lower():
                                integration_cnt += 1
                            elif "e2e" in file_name or "selenium" in content.lower() or "cypress" in content.lower():
                                e2e_cnt += 1
                            else:
                                unit_cnt += 1
                    except Exception:
                        pass

        if unit_cnt > 0: self.testing_registry["unit"] = f"{unit_cnt} Verified suites"
        if integration_cnt > 0: self.testing_registry["integration"] = f"{integration_cnt} Verified suites"
        if e2e_cnt > 0: self.testing_registry["e2e"] = f"{e2e_cnt} Verified suites"

        # Check for junit XML or coverage report files inside target workspace
        junit_glob = list(self.repo_path.glob("**/junit.xml")) + list(self.repo_path.glob("**/test-results.xml"))
        if junit_glob:
            relative_report = str(junit_glob[0].relative_to(self.repo_path)).replace("\\", "/")
            self.testing_registry["evidence"] = f"File: {relative_report}"
            try:
                tree = ET.parse(junit_glob[0])
                root = tree.getroot()
                tests = 0
                failures = 0
                errors = 0
                
                if root.tag == "testsuite":
                    tests = int(root.attrib.get("tests", 0))
                    failures = int(root.attrib.get("failures", 0))
                    errors = int(root.attrib.get("errors", 0))
                elif root.tag == "testsuites":
                    tests = int(root.attrib.get("tests", 0))
                    failures = int(root.attrib.get("failures", 0))
                    errors = int(root.attrib.get("errors", 0))
                    for ts in root.findall(".//testsuite"):
                        tests += int(ts.attrib.get("tests", 0))
                        failures += int(ts.attrib.get("failures", 0))
                        errors += int(ts.attrib.get("errors", 0))
                        
                if tests > 0:
                    passed = tests - failures - errors
                    pass_rate = (passed / tests) * 100
                    self.testing_registry["pass_rate"] = f"{pass_rate:.1f}%"
                else:
                    self.testing_registry["pass_rate"] = "UNKNOWN"
            except Exception:
                self.testing_registry["pass_rate"] = "UNKNOWN"
            
        coverage_glob = list(self.repo_path.glob("**/coverage.xml")) + list(self.repo_path.glob("**/cobertura.xml"))
        if coverage_glob:
            relative_cov = str(coverage_glob[0].relative_to(self.repo_path)).replace("\\", "/")
            self.testing_registry["evidence"] = f"File: {relative_cov}"
            try:
                tree = ET.parse(coverage_glob[0])
                root = tree.getroot()
                line_rate = root.attrib.get("line-rate")
                if line_rate:
                    cov_pct = float(line_rate) * 100
                    self.testing_registry["coverage"] = f"{cov_pct:.1f}% (VERIFIED from {relative_cov})"
                else:
                    self.testing_registry["coverage"] = "UNKNOWN"
            except Exception:
                self.testing_registry["coverage"] = "UNKNOWN"

    def _verify_operational_benchmarks(self):
        perf_glob = list(self.repo_path.glob("**/benchmark_results.json")) + list(self.repo_path.glob("**/perf_report.md"))
        if perf_glob:
            relative_perf = str(perf_glob[0].relative_to(self.repo_path)).replace("\\", "/")
            self.testing_registry["performance"] = f"VERIFIED: {relative_perf}"
            
            if perf_glob[0].suffix.lower() == ".json":
                import json
                try:
                    data = json.loads(perf_glob[0].read_text(encoding="utf-8", errors="ignore"))
                    latency = data.get("latency_ms") or data.get("average_latency") or data.get("latency")
                    if latency:
                        self.testing_registry["performance"] = f"{latency}ms (VERIFIED from {relative_perf})"
                except Exception:
                    pass

    def _calculate_factual_scores(self):
        # Default scores strictly to UNKNOWN under v3.1 unless proven by reports
        self.metrics["security_score"] = "UNKNOWN"
        self.metrics["quality_score"] = "UNKNOWN"
        self.metrics["reliability_score"] = "UNKNOWN"
        self.metrics["complexity_score"] = "UNKNOWN"
