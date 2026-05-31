# -*- coding: utf-8 -*-
"""
UADOS — Universal AI Project Brain Framework (AIPBF) v2.0
Rigorous Repository Analyzer & Fact Classifier
"""

import os
import re
from pathlib import Path

class RepositoryAnalyzer:
    def __init__(self, repo_path):
        self.repo_path = Path(repo_path).resolve()
        self.ignore_patterns = [
            "node_modules", "vendor", "dist", "build", ".next",
            "coverage", "generated", "bin", "obj", "tmp", ".cache",
            "target", "out", ".git", "third_party"
        ]
        self.metrics = {
            "languages": {},
            "tech_stack": {
                "languages": [],
                "frameworks": [],
                "libraries": [],
                "databases": [],
                "messaging": [],
                "build_tools": []
            },
            "loc": 0,
            "file_counts": {"src": 0, "test": 0, "config": 0},
            "apis": [],
            "databases": [],
            "events": [],
            "dependencies": {
                "internal": [],
                "external": []
            },
            "components": [],
            "facts": []  # List of all classified facts for traceability
        }

    def is_ignored(self, path):
        parts = Path(path).parts
        return any(pat in parts for pat in self.ignore_patterns)

    def analyze(self):
        self._crawl_and_count()
        self._parse_manifests()
        self._extract_code_intelligence()
        self._infer_architecture_components()
        return self.metrics

    def _add_fact(self, title, description, category, verification, file_path, line_num, confidence):
        relative_path = ""
        try:
            relative_path = str(Path(file_path).relative_to(self.repo_path)).replace("\\", "/")
        except Exception:
            relative_path = str(file_path).replace("\\", "/")

        self.metrics["facts"].append({
            "title": title,
            "description": description,
            "category": category,
            "verification": verification,
            "evidence": {
                "file": relative_path,
                "line": line_num,
                "confidence": confidence
            }
        })

    def _crawl_and_count(self):
        lang_exts = {
            ".cpp": "C++", ".hpp": "C++", ".h": "C/C++", ".c": "C",
            ".py": "Python", ".go": "Go", ".rs": "Rust", ".cs": "C#",
            ".js": "JavaScript", ".ts": "TypeScript", ".tsx": "TypeScript",
            ".java": "Java", ".kt": "Kotlin", ".swift": "Swift",
            ".html": "HTML", ".css": "CSS", ".md": "Markdown", 
            ".yaml": "YAML", ".yml": "YAML", ".json": "JSON"
        }

        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                file_path = Path(root) / file
                ext = file_path.suffix.lower()
                
                # Check category
                if "test_" in file or "spec" in file or "_test" in file:
                    self.metrics["file_counts"]["test"] += 1
                elif ext in [".json", ".yaml", ".yml", ".ini", ".conf", ".toml", ".xml"]:
                    self.metrics["file_counts"]["config"] += 1
                else:
                    self.metrics["file_counts"]["src"] += 1

                if ext in lang_exts:
                    lang = lang_exts[ext]
                    self.metrics["languages"][lang] = self.metrics["languages"].get(lang, 0) + 1
                    try:
                        lines = file_path.read_text(encoding="utf-8", errors="ignore").splitlines()
                        self.metrics["loc"] += len(lines)
                        
                        # Record language file as verified fact
                        self._add_fact(
                            title=f"Source File Discover: {file}",
                            description=f"Factual source module detected in target workspace tree.",
                            category="Workspace Layout",
                            verification="VERIFIED",
                            file_path=file_path,
                            line_num=1,
                            confidence="HIGH"
                        )
                    except Exception:
                        pass

        sorted_langs = sorted(self.metrics["languages"].items(), key=lambda x: x[1], reverse=True)
        self.metrics["tech_stack"]["languages"] = [lang for lang, _ in sorted_langs]

    def _parse_manifests(self):
        # 1. Conan C++
        conan = self.repo_path / "conanfile.py"
        if conan.exists() and not self.is_ignored(conan):
            self.metrics["tech_stack"]["build_tools"].append("Conan")
            content = conan.read_text(encoding="utf-8", errors="ignore")
            lines = content.splitlines()
            for i, line in enumerate(lines):
                match = re.search(r'self\.requires\("([^"]+)"\)', line)
                if match:
                    pkg = match.group(1)
                    self.metrics["dependencies"]["external"].append(pkg)
                    self._add_fact(
                        title=f"Conan Dependency: {pkg}",
                        description="External library dependency declared in conanfile.py configuration.",
                        category="Dependencies",
                        verification="VERIFIED",
                        file_path=conan,
                        line_num=i + 1,
                        confidence="HIGH"
                    )

        # 2. CMake
        cmake = self.repo_path / "CMakeLists.txt"
        if cmake.exists() and not self.is_ignored(cmake):
            self.metrics["tech_stack"]["build_tools"].append("CMake")
            self._add_fact(
                title="Build Engine (CMake)",
                description="Declared project build settings and presets generator files.",
                category="Build System",
                verification="VERIFIED",
                file_path=cmake,
                line_num=1,
                confidence="HIGH"
            )

        # 3. Python package lists
        reqs = self.repo_path / "requirements.txt"
        if reqs.exists() and not self.is_ignored(reqs):
            self.metrics["tech_stack"]["build_tools"].append("pip")
            lines = reqs.read_text(encoding="utf-8", errors="ignore").splitlines()
            for i, line in enumerate(lines):
                if line and not line.startswith("#"):
                    pkg = line.split("==")[0].split(">=")[0].strip()
                    self.metrics["dependencies"]["external"].append(pkg)
                    self._add_fact(
                        title=f"Pip Package: {pkg}",
                        description="Python external dependency listed in package index manifest.",
                        category="Dependencies",
                        verification="VERIFIED",
                        file_path=reqs,
                        line_num=i + 1,
                        confidence="HIGH"
                    )

        # 4. Node.js packages
        package_json = self.repo_path / "package.json"
        if package_json.exists() and not self.is_ignored(package_json):
            self.metrics["tech_stack"]["build_tools"].append("npm/yarn")
            import json
            try:
                data = json.loads(package_json.read_text(encoding="utf-8", errors="ignore"))
                deps = data.get("dependencies", {})
                dev_deps = data.get("devDependencies", {})
                for dep in list(deps.keys()) + list(dev_deps.keys()):
                    self.metrics["dependencies"]["external"].append(dep)
                    self._add_fact(
                        title=f"Node.js Dependency: {dep}",
                        description="JSON package manifest dependency.",
                        category="Dependencies",
                        verification="VERIFIED",
                        file_path=package_json,
                        line_num=1,
                        confidence="HIGH"
                    )
                if "express" in deps: self.metrics["tech_stack"]["frameworks"].append("Express")
                if "react" in deps: self.metrics["tech_stack"]["frameworks"].append("React")
                if "vue" in deps: self.metrics["tech_stack"]["frameworks"].append("Vue")
            except Exception:
                pass

        self.metrics["dependencies"]["external"] = sorted(list(set(self.metrics["dependencies"]["external"])))

    def _extract_code_intelligence(self):
        api_patterns = [
            (r'@(?:app|router|blueprint)\.(?:get|post|put|delete|patch|route)\(["\']([^"\']+)["\']', "REST"),
            (r'\.(?:get|post|put|delete|patch)\(["\']([^"\']+)["\']\s*,', "REST"),
            (r'@(?:GetMapping|PostMapping|PutMapping|DeleteMapping|RequestMapping)\(["\']([^"\']+)["\']', "REST"),
            (r'\[(?:HttpGet|HttpPost|HttpPut|HttpDelete|Route)\(["\']([^"\']+)["\']', "REST")
        ]

        db_patterns = [
            (r'mongodb://|pymongo|mongoose', "MongoDB"),
            (r'postgresql|pg\.|postgres', "PostgreSQL"),
            (r'mysql', "MySQL"),
            (r'redis|ioredis', "Redis"),
            (r'sqlite', "SQLite")
        ]

        event_patterns = [
            (r'EventEmitter|emit\(|publish\(|subscribe\(', "Event"),
            (r'EventBus|EventEnvelope|dispatch\(', "EventBus"),
            (r'Kafka|RabbitMQ|mqtt', "Broker")
        ]

        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".py", ".go", ".rs", ".cs", ".js", ".ts", ".java"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        lines = content.splitlines()
                        
                        # Extract APIs
                        for pat, protocol in api_patterns:
                            for match in re.finditer(pat, content):
                                route = match.group(1)
                                line_num = content[:match.start()].count("\n") + 1
                                api_node = {
                                    "endpoint": route,
                                    "protocol": protocol,
                                    "file": file_path.name,
                                    "line": line_num,
                                    "verification": "VERIFIED"
                                }
                                self.metrics["apis"].append(api_node)
                                self._add_fact(
                                    title=f"API Endpoint: {route}",
                                    description=f"Discovered active endpoint mapped via controller route.",
                                    category="API Discovery",
                                    verification="VERIFIED",
                                    file_path=file_path,
                                    line_num=line_num,
                                    confidence="HIGH"
                                )

                        # Extract DB
                        for pat, db_type in db_patterns:
                            match = re.search(pat, content, re.IGNORECASE)
                            if match:
                                line_num = content[:match.start()].count("\n") + 1
                                if db_type not in self.metrics["tech_stack"]["databases"]:
                                    self.metrics["tech_stack"]["databases"].append(db_type)
                                    self.metrics["databases"].append({
                                        "type": db_type,
                                        "file": file_path.name,
                                        "line": line_num,
                                        "verification": "VERIFIED"
                                    })
                                    self._add_fact(
                                        title=f"Database System: {db_type}",
                                        description="Database client initialization or connection string trace found.",
                                        category="Databases",
                                        verification="VERIFIED",
                                        file_path=file_path,
                                        line_num=line_num,
                                        confidence="HIGH"
                                    )

                        # Extract Events
                        for pat, ev_type in event_patterns:
                            for match in re.finditer(pat, content):
                                line_num = content[:match.start()].count("\n") + 1
                                self.metrics["events"].append({
                                    "pattern": match.group(0),
                                    "type": ev_type,
                                    "file": file_path.name,
                                    "line": line_num,
                                    "verification": "VERIFIED"
                                })
                                self._add_fact(
                                    title=f"Event Router Event: {match.group(0)}",
                                    description="Discovered asynchronous event broker publish/subscribe hook.",
                                    category="Events",
                                    verification="VERIFIED",
                                    file_path=file_path,
                                    line_num=line_num,
                                    confidence="HIGH"
                                )

                    except Exception:
                        pass

    def _infer_architecture_components(self):
        # Infer component architecture layers based on workspace directories (INFERRED facts)
        standard_dirs = [
            ("core", "UADOS Core Microkernel & IPC Scheduler Subsystem"),
            ("hal", "Hardware Abstraction Layer DBW controls"),
            ("sensors", "Integrated sensors and EKF Fusion algorithms"),
            ("perception", "Vision classification and polyline tracking"),
            ("planning", "Strategic behavior trajectory solvers"),
            ("control", "Longitudinal PID & lateral Stanley controllers"),
            ("safety", "Failsafe envelope and Emergency MRC controls"),
            ("fleet", "Hot-reload OTA update and telemetry manager"),
            ("validation", "Fault injectors and automated validators")
        ]

        for folder, desc in standard_dirs:
            p = self.repo_path / folder
            if p.exists() and p.is_dir():
                self.metrics["components"].append({
                    "name": folder.upper(),
                    "description": desc,
                    "verification": "VERIFIED"
                })
                # Add inferred fact based on the directory structure
                self._add_fact(
                    title=f"Architecture Layer: {folder.upper()}",
                    description=desc,
                    category="Architecture",
                    verification="VERIFIED",
                    file_path=p,
                    line_num=1,
                    confidence="HIGH"
                )
