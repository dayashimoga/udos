# -*- coding: utf-8 -*-
"""
UADOS — Universal AI Project Brain Framework (AIPBF)
Heuristic Code Review & Quality Auditor Engine
"""

import os
import re
from pathlib import Path

class RepositoryReviewer:
    def __init__(self, repo_path):
        self.repo_path = Path(repo_path).resolve()
        self.findings = []
        self.vulnerabilities = []
        self.debt_items = []
        self.metrics = {
            "security_score": 100,
            "quality_score": 100,
            "reliability_score": 100,
            "complexity_score": 10
        }
        self.ignore_patterns = ["build", ".venv", "node_modules", ".git", "third_party", "bin", "obj"]

    def is_ignored(self, path):
        parts = Path(path).parts
        return any(pat in parts for pat in self.ignore_patterns)

    def review(self):
        self._audit_codebase()
        self._calculate_scores()
        return {
            "scores": self.metrics,
            "findings": self.findings,
            "vulnerabilities": self.vulnerabilities,
            "debt": self.debt_items
        }

    def _audit_codebase(self):
        # 1. Regex definitions for audit
        secret_patterns = [
            (r'(?i)(?:key|secret|password|passwd|token|credential|keyfile)\s*=\s*["\'][a-zA-Z0-9_\-\.\/]{8,}["\']', "Hardcoded Secrets"),
            (r'(?i)private_key\s*=\s*["\']-+BEGIN', "Private Key disclosure")
        ]

        unsafe_patterns = [
            (r'\beval\([^)]*\)', "Use of dynamic code execution (eval)", "High", "Security"),
            (r'\bsystem\([^)]*\)', "Use of shell command execution (system)", "Medium", "Security"),
            (r'\bstrcpy\b', "Use of unsafe buffer function (strcpy)", "Medium", "Reliability"),
            (r'\bprintf\b', "Raw console printf instead of thread-safe logger", "Low", "Quality")
        ]

        mrc_or_fallback = r'emergency|recovery|rollback|fallback|MRC|safe_stop'

        # Crawl codebase
        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".py", ".go", ".rs", ".cs", ".js", ".ts", ".java"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        lines = content.splitlines()

                        # Size checks (complexity heuristic)
                        if len(lines) > 800:
                            self.debt_items.append({
                                "file": file_path.name,
                                "type": "Large Source File Complexity",
                                "impact": "High complexity, hard to maintain",
                                "priority": "Medium",
                                "refactor": f"Split {file_path.name} into smaller cohesive classes/modules."
                            })

                        # Secrets Audit
                        for pat, title in secret_patterns:
                            matches = re.finditer(pat, content)
                            for match in matches:
                                line_num = content[:match.start()].count("\n") + 1
                                self.vulnerabilities.append({
                                    "file": file_path.name,
                                    "line": line_num,
                                    "title": title,
                                    "severity": "CRITICAL",
                                    "fix": "Move secret value to environment variables or local gitignored YAML configuration file."
                                })

                        # Unsafe Methods & Code Quality Audit
                        for pat, desc, severity, category in unsafe_patterns:
                            matches = re.finditer(pat, content)
                            for match in matches:
                                line_num = content[:match.start()].count("\n") + 1
                                self.findings.append({
                                    "file": file_path.name,
                                    "line": line_num,
                                    "category": category,
                                    "severity": severity,
                                    "description": desc,
                                    "fix": "Replace with secure library alternative."
                                })

                        # Check for hardcoded simulated/mock fallbacks
                        if "mock" in file.lower() or "simulate" in file.lower() or re.search(r'load_mock|mock_data|synthetic', content, re.IGNORECASE):
                            self.findings.append({
                                "file": file_path.name,
                                "category": "Simulation Mock",
                                "severity": "Low",
                                "description": "Static simulation/mock data loaded in operational execution pathway.",
                                "fix": "Ensure model or map backends are gateable behind compile flags."
                            })

                    except Exception:
                        pass

    def _calculate_scores(self):
        # 1. Security Score
        crit_vulns = sum(1 for v in self.vulnerabilities if v["severity"] == "CRITICAL")
        high_vulns = sum(1 for f in self.findings if f["severity"] == "High" and f["category"] == "Security")
        med_vulns = sum(1 for f in self.findings if f["severity"] == "Medium" and f["category"] == "Security")

        self.metrics["security_score"] = max(0, 100 - (crit_vulns * 25) - (high_vulns * 15) - (med_vulns * 5))

        # 2. Quality & Maintainability Score
        large_files = len(self.debt_items)
        low_findings = sum(1 for f in self.findings if f["severity"] == "Low")
        self.metrics["quality_score"] = max(0, 100 - (large_files * 10) - (low_findings * 2))

        # 3. Reliability Score
        unsafe_funcs = sum(1 for f in self.findings if f["category"] == "Reliability")
        self.metrics["reliability_score"] = max(0, 100 - (unsafe_funcs * 10))

        # 4. Complexity Heuristic
        self.metrics["complexity_score"] = min(100, 10 + (large_files * 15) + (len(self.findings) * 2))
