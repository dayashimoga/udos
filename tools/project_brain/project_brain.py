#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Universal AI Project Brain Framework (AIPBF) — CLI CLI Entrypoint
"""

import sys
import argparse
from pathlib import Path

from analyzer import RepositoryAnalyzer
from reviewer import RepositoryReviewer
from generator import DocumentationGenerator

def main():
    parser = argparse.ArgumentParser(description="Universal AI Project Brain Framework (AIPBF) CLI")
    parser.add_argument("--scan", action="store_true", help="Scan repository and auto-generate/update PROJECT_BRAIN.md")
    parser.add_argument("--review", action="store_true", help="Perform static security, reliability, and code quality audit")
    parser.add_argument("--init", action="store_true", help="Initialize the AI_BRAIN and docs folders in target repository")
    parser.add_argument("--path", type=str, default=".", help="Repository root path to analyze")

    args = parser.parse_args()
    repo_path = Path(args.path).resolve()

    if not repo_path.exists():
        print(f"Error: Target path '{args.path}' does not exist!")
        sys.exit(1)

    print("====== Universal AI Project Brain Framework (AIPBF) ======")
    print(f"Target Repository: {repo_path}")
    print("==========================================================")

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
    reviewer = RepositoryReviewer(repo_path)
    review_data = reviewer.review()
    
    scores = review_data["scores"]
    print(f"[AIPBF] Security Score: {scores['security_score']}%")
    print(f"[AIPBF] Quality Score: {scores['quality_score']}%")
    print(f"[AIPBF] Reliability Score: {scores['reliability_score']}%")

    if args.review:
        print("\n====== Dynamic Audit Findings ======")
        for find in review_data["findings"]:
            line_info = f":L{find['line']}" if "line" in find else ""
            print(f"- [{find['category']}] {find['file']}{line_info} -> {find['description']} ({find['severity']})")
        print("\n====== Detected Vulnerabilities ======")
        for vuln in review_data["vulnerabilities"]:
            print(f"- [VULNERABILITY] {vuln['file']}:{vuln['line']} -> {vuln['title']} ({vuln['severity']})")
        print("\n====== Technical Debt Registries ======")
        for debt in review_data["debt"]:
            print(f"- [DEBT] {debt['file']} -> {debt['type']} ({debt['priority']})")
        print("====================================")

    if args.scan or args.init or not (args.review):
        print("\n[AIPBF] Rendering Markdown master guides...")
        generator = DocumentationGenerator(repo_path, analysis_data, review_data)
        generator.generate_all()
        print("[AIPBF] Documentation system synchronizations complete!")

    print("\n====== AIPBF Execution Complete! ======")

if __name__ == "__main__":
    main()
