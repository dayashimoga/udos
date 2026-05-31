# -*- coding: utf-8 -*-
"""
UADOS — Universal AI Project Brain Framework (AIPBF)
Repository Analyzer & Intelligence Crawler
"""

import os
import re
from pathlib import Path

class RepositoryAnalyzer:
    def __init__(self, repo_path):
        self.repo_path = Path(repo_path).resolve()
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
            }
        }
        self.ignore_patterns = ["build", ".venv", "node_modules", ".git", "third_party", "bin", "obj"]

    def is_ignored(self, path):
        parts = Path(path).parts
        return any(pat in parts for pat in self.ignore_patterns)

    def analyze(self):
        self._crawl_and_count()
        self._parse_manifests()
        self._extract_code_intelligence()
        return self.metrics

    def _crawl_and_count(self):
        lang_exts = {
            ".cpp": "C++", ".hpp": "C++", ".h": "C/C++", ".c": "C",
            ".py": "Python", ".go": "Go", ".rs": "Rust", ".cs": "C#",
            ".js": "JavaScript", ".ts": "TypeScript", ".tsx": "TypeScript",
            ".java": "Java", ".kt": "Kotlin", ".swift": "Swift",
            ".rb": "Ruby", ".php": "PHP", ".html": "HTML", ".css": "CSS",
            ".md": "Markdown", ".yaml": "YAML", ".yml": "YAML", ".json": "JSON"
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
                        loc = len(file_path.read_text(encoding="utf-8", errors="ignore").splitlines())
                        self.metrics["loc"] += loc
                    except Exception:
                        pass

        # Sort languages by frequency
        sorted_langs = sorted(self.metrics["languages"].items(), key=lambda x: x[1], reverse=True)
        self.metrics["tech_stack"]["languages"] = [lang for lang, _ in sorted_langs]

    def _parse_manifests(self):
        # 1. Python
        reqs = self.repo_path / "requirements.txt"
        if reqs.exists():
            self.metrics["tech_stack"]["build_tools"].append("pip")
            for line in reqs.read_text(encoding="utf-8", errors="ignore").splitlines():
                if line and not line.startswith("#"):
                    pkg = line.split("==")[0].split(">=")[0].strip()
                    self.metrics["dependencies"]["external"].append(pkg)

        # 2. Node.js
        package_json = self.repo_path / "package.json"
        if package_json.exists():
            self.metrics["tech_stack"]["build_tools"].append("npm/yarn")
            import json
            try:
                data = json.loads(package_json.read_text(encoding="utf-8", errors="ignore"))
                deps = data.get("dependencies", {})
                dev_deps = data.get("devDependencies", {})
                for dep in list(deps.keys()) + list(dev_deps.keys()):
                    self.metrics["dependencies"]["external"].append(dep)
                # Framework heuristics
                if "express" in deps: self.metrics["tech_stack"]["frameworks"].append("Express")
                if "react" in deps: self.metrics["tech_stack"]["frameworks"].append("React")
                if "vue" in deps: self.metrics["tech_stack"]["frameworks"].append("Vue")
            except Exception:
                pass

        # 3. C++ Conan & CMake
        conan = self.repo_path / "conanfile.py"
        if conan.exists():
            self.metrics["tech_stack"]["build_tools"].append("Conan")
            content = conan.read_text(encoding="utf-8", errors="ignore")
            for req in re.findall(r'self\.requires\("([^"]+)"\)', content):
                self.metrics["dependencies"]["external"].append(req)

        cmake = self.repo_path / "CMakeLists.txt"
        if cmake.exists():
            self.metrics["tech_stack"]["build_tools"].append("CMake")

        # Deduplicate and sort dependencies
        self.metrics["dependencies"]["external"] = sorted(list(set(self.metrics["dependencies"]["external"])))

    def _extract_code_intelligence(self):
        api_patterns = [
            # Python
            (r'@(?:app|router|blueprint)\.(?:get|post|put|delete|patch|route)\(["\']([^"\']+)["\']', "REST"),
            # Express / JS
            (r'\.(?:get|post|put|delete|patch)\(["\']([^"\']+)["\']\s*,', "REST"),
            # Spring / Java
            (r'@(?:GetMapping|PostMapping|PutMapping|DeleteMapping|RequestMapping)\(["\']([^"\']+)["\']', "REST"),
            # ASP.NET / C#
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
                        
                        # Extract APIs
                        for pat, protocol in api_patterns:
                            for route in re.findall(pat, content):
                                self.metrics["apis"].append({
                                    "endpoint": route,
                                    "protocol": protocol,
                                    "file": file_path.name
                                })

                        # Extract DB Connections
                        for pat, db_type in db_patterns:
                            if re.search(pat, content, re.IGNORECASE):
                                if db_type not in self.metrics["tech_stack"]["databases"]:
                                    self.metrics["tech_stack"]["databases"].append(db_type)
                                    self.metrics["databases"].append({
                                        "type": db_type,
                                        "file": file_path.name
                                    })

                        # Extract Event Publishers/Consumers
                        for pat, ev_type in event_patterns:
                            if re.search(pat, content, re.IGNORECASE):
                                match = re.search(pat, content)
                                self.metrics["events"].append({
                                    "pattern": match.group(0),
                                    "type": ev_type,
                                    "file": file_path.name
                                })

                    except Exception:
                        pass

        # Deduplicate results
        self.metrics["apis"] = self.metrics["apis"][:30] # Limit to top 30
        self.metrics["events"] = self.metrics["events"][:30]
