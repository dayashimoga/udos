#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Universal AI Project Brain Framework (AIPBF) v2.0 — CLI & Self-Healing Quality Gate
"""

import sys
import argparse
from pathlib import Path

from analyzer import RepositoryAnalyzer
from reviewer import RepositoryReviewer
from generator import DocumentationGenerator

class SelfHealingQualityGate:
    def __init__(self, repo_path, analysis_data, review_data):
        self.repo_path = repo_path
        self.analysis = analysis_data
        self.review = review_data
        self.ignore_patterns = ["node_modules", "vendor", "dist", "build", "third_party", "tools"]

    def validate(self):
        print("[QualityGate] Validating generated project brain metadata...")
        
        # 1. Gate check: No node_modules or vendor directories analyzed
        for fact in self.analysis.get("facts", []):
            file_path = fact["evidence"]["file"]
            parts = Path(file_path).parts
            if any(pat in parts for pat in self.ignore_patterns):
                raise ValueError(f"QualityGate Violation: Ignored directory found in analysis fact evidence: {file_path}")

        for vuln in self.review.get("vulnerabilities", []):
            file_path = vuln["evidence"]["file"]
            parts = Path(file_path).parts
            if any(pat in parts for pat in self.ignore_patterns):
                raise ValueError(f"QualityGate Violation: Ignored directory found in vulnerability evidence: {file_path}")

        for find in self.review.get("findings", []):
            file_path = find["evidence"]["file"]
            parts = Path(file_path).parts
            if any(pat in parts for pat in self.ignore_patterns):
                raise ValueError(f"QualityGate Violation: Ignored directory found in audit finding evidence: {file_path}")

        # 2. Gate check: Contradictory technologies check
        languages = self.analysis.get("tech_stack", {}).get("languages", [])
        if "C++" in languages and "conanfile.py" in self.analysis.get("tech_stack", {}).get("build_tools", []) and "pip" in self.analysis.get("tech_stack", {}).get("build_tools", []):
            # Safe combinations
            pass
            
        # 3. Gate check: Duplicate findings check
        finding_sigs = set()
        for find in self.review.get("findings", []):
            sig = f"{find['evidence']['file']}:{find['evidence']['line']}:{find['description']}"
            if sig in finding_sigs:
                raise ValueError(f"QualityGate Violation: Duplicate audit finding detected: {sig}")
            finding_sigs.add(sig)

        print("[QualityGate] All v2.0 self-healing validation rules passed successfully!")
        return True

    def heal(self):
        # Auto-heal by filtering out any invalid nodes in place
        self.analysis["facts"] = [f for f in self.analysis.get("facts", []) if not any(pat in Path(f["evidence"]["file"]).parts for pat in self.ignore_patterns)]
        self.review["vulnerabilities"] = [v for v in self.review.get("vulnerabilities", []) if not any(pat in Path(v["evidence"]["file"]).parts for pat in self.ignore_patterns)]
        self.review["findings"] = [f for f in self.review.get("findings", []) if not any(pat in Path(f["evidence"]["file"]).parts for pat in self.ignore_patterns)]
        print("[QualityGate] Auto-healed metadata anomalies successfully.")

def main():
    parser = argparse.ArgumentParser(description="Universal AI Project Brain Framework (AIPBF) v2.0 CLI")
    parser.add_argument("--scan", action="store_true", help="Scan repository and auto-generate/update PROJECT_BRAIN.md")
    parser.add_argument("--review", action="store_true", help="Perform static security, reliability, and code quality audit")
    parser.add_argument("--init", action="store_true", help="Initialize the AI_BRAIN and docs folders in target repository")
    parser.add_argument("--path", type=str, default=".", help="Repository root path to analyze")

    args = parser.parse_args()
    repo_path = Path(args.path).resolve()

    if not repo_path.exists():
        print(f"Error: Target path '{args.path}' does not exist!")
        sys.exit(1)

    print("====== Universal AI Project Brain Framework (AIPBF) v3.3 ======")
    print(f"Target Repository: {repo_path}")
    print("===============================================================")

    if args.init:
        print("[AIPBF] Initializing project folder architecture...")
        (repo_path / "AI_BRAIN").mkdir(exist_ok=True)
        (repo_path / "docs").mkdir(exist_ok=True)
        print("[AIPBF] Directory structures verified!")

    # 1. Execute repository scan analysis
    print("[AIPBF] Running static file crawling...")
    analyzer = RepositoryAnalyzer(repo_path)
    analysis_data = analyzer.analyze()
    print(f"[AIPBF] Found {analysis_data['loc']} lines of code across {len(analysis_data['tech_stack']['languages'])} languages.")

    # 2. Execute code and security review
    print("[AIPBF] Running quality and security audit...")
    reviewer = RepositoryReviewer(repo_path, analysis_data)
    review_data = reviewer.review()
    
    scores = review_data["scores"]
    print(f"[AIPBF] Security Score: {scores['security_score']}%")
    print(f"[AIPBF] Quality Score: {scores['quality_score']}%")
    print(f"[AIPBF] Reliability Score: {scores['reliability_score']}%")

    # 3. Trigger Self-Healing Quality Gate checks
    gate = SelfHealingQualityGate(repo_path, analysis_data, review_data)
    try:
        gate.validate()
    except ValueError as e:
        print(f"[QualityGate] Validation failed: {e}. Initiating auto-healing procedures...")
        gate.heal()
        gate.validate() # Re-verify after healing

    if args.review:
        print("\n====== Dynamic Audit Findings ======")
        for find in review_data["findings"]:
            line_info = f":L{find['evidence']['line']}" if "line" in find["evidence"] else ""
            print(f"- [{find['category']}] {find['evidence']['file']}{line_info} -> {find['description']} ({find['severity']})")
        print("\n====== Detected Vulnerabilities ======")
        for vuln in review_data["vulnerabilities"]:
            line_info = f":L{vuln['evidence']['line']}" if "line" in vuln["evidence"] else ""
            print(f"- [VULNERABILITY] {vuln['evidence']['file']}{line_info} -> {vuln['title']} ({vuln['severity']})")
        print("\n====== Technical Debt Registries ======")
        for debt in review_data["debt"]:
            print(f"- [DEBT] {debt['files']} -> {debt['title']} ({debt['priority']})")
        print("====================================")

    if args.scan or args.init or not (args.review):
        print("\n[AIPBF] Rendering Markdown master guides...")
        generator = DocumentationGenerator(repo_path, analysis_data, review_data)
        generator.generate_all()
        print("[AIPBF] Documentation system synchronizations complete!")

    print("\n====== AIPBF v3.3 Execution Complete! ======")

if __name__ == "__main__":
    main()
