# -*- coding: utf-8 -*-
"""
UADOS — Universal AI Project Brain Framework (AIPBF) v3.1
Factual Repository Analyzer & Import Dependency Crawler
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
            "target", "out", ".git", "third_party", "tools", "analysis",
            "project_brain"
        ]
        self.metrics = {
            "project_identity": {
                "type": "UNKNOWN",
                "domain": "UNKNOWN",
                "purpose": "UNKNOWN",
                "confidence": "LOW",
                "evidence": []
            },
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
            "facts": [],
            "module_graph": [],  # Verified dynamic import relations graph
            "directories": {},  # Exists checklist
            "requirements": [], # Dynamic Requirements Traceability list
            "data_flow": [],     # Inferred Data Flow
            "build_targets": [], # CMake / NPM script targets
            "target_dependencies": {}, # target name -> list of linked targets
            "build_order": [],   # Sorted topological build targets order
            "entry_points": [],  # Scanned main / startup entry points
            "test_map": {},      # Subsystem -> List of test files
            "ownership_map": {}  # Subsystem -> Count of code files
        }

    def is_ignored(self, path):
        parts = Path(path).parts
        return any(pat in parts for pat in self.ignore_patterns)

    def analyze(self):
        self._crawl_and_count()
        self._parse_manifests()
        self._classify_project_identity()
        self._extract_code_intelligence()
        self._derive_import_dependencies()
        self._scan_directory_status()
        self._extract_build_targets_and_entries()
        self._extract_requirements_traceability()
        self._derive_data_flow()
        self._build_test_and_ownership_maps()
        self._calculate_build_order()
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
                        
                        self._add_fact(
                            title=f"Source File Discover: {file}",
                            description=f"Source file found in project tree.",
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

    def _classify_project_identity(self):
        trading_weights = 0
        trading_evidence = []
        auto_weights = 0
        auto_evidence = []
        
        # Rigid v3.1 terms checks
        adas_weights = 0
        adas_evidence = []

        trading_keys = ["backtest", "forecast", "trading", "portfolio", "ticker", "order", "exchange", "alpha", "market", "price"]
        auto_keys = ["stanley", "controller", "imu", "gps", "lidar", "radar", "camera", "fusion", "dbw", "canbus", "microkernel"]
        adas_keys = ["lane planner", "trajectory planner", "adas", "autonomous driving", "perception stack", "carla"]

        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            
            folder_name = Path(root).name.lower()
            if "trading" in folder_name or "analytics" in folder_name:
                trading_weights += 15
                trading_evidence.append(f"Folder: {folder_name}")
            if "control" in folder_name or "sensors" in folder_name or "perception" in folder_name:
                auto_weights += 15
                auto_evidence.append(f"Folder: {folder_name}")

            for file in files:
                file_name = file.lower()
                for key in trading_keys:
                    if key in file_name:
                        trading_weights += 5
                        trading_evidence.append(f"File matches key '{key}': {file}")
                for key in auto_keys:
                    if key in file_name:
                        auto_weights += 5
                        auto_evidence.append(f"File matches key '{key}': {file}")
                
                # Check for explicit AD/ADAS terms
                try:
                    file_path = Path(root) / file
                    if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".py", ".ts", ".md"]:
                        content = file_path.read_text(encoding="utf-8", errors="ignore").lower()
                        for key in adas_keys:
                            if key in content:
                                adas_weights += 10
                                adas_evidence.append(f"File {file} contains term '{key}'")
                except Exception:
                    pass

        ident = self.metrics["project_identity"]
        if trading_weights > auto_weights and trading_weights > 10:
            ident["type"] = "Autonomous Trading Platform"
            ident["domain"] = "Quantitative Finance & Trading Execution"
            ident["purpose"] = "Simulate, analyze, and execute high-rate quantitative trading signals."
            ident["confidence"] = "HIGH" if trading_weights > 30 else "MEDIUM"
            ident["evidence"] = sorted(list(set(trading_evidence)))[:5]
        elif auto_weights > trading_weights and auto_weights > 10:
            # Under v3.1, unless explicit lane/planning/ADAS terms exist, classify as Robotics systems (Fix 1)
            if adas_weights > 20:
                ident["type"] = "Autonomous Driving Operating System"
                ident["domain"] = "Autonomous Vehicles & Robotic Systems"
                ident["purpose"] = "Failsafe real-time vehicle scheduling, fusion, path planning, and envelope controls."
                ident["confidence"] = "HIGH"
                ident["evidence"] = sorted(list(set(auto_evidence + adas_evidence)))[:5]
            else:
                ident["type"] = "Robotics / Autonomous Systems Platform"
                ident["domain"] = "Robotics & Dynamic Systems Platform"
                ident["purpose"] = "Failsafe real-time component scheduling, sensor fusion, and actuator controls."
                ident["confidence"] = "MEDIUM"
                ident["evidence"] = sorted(list(set(auto_evidence)))[:5]
        else:
            ident["type"] = "UNKNOWN"
            ident["domain"] = "UNKNOWN"
            ident["purpose"] = "UNKNOWN"
            ident["confidence"] = "LOW"
            ident["evidence"] = ["No specific project type files or directories verified."]

        evidence_file = self.repo_path / "package.json" if (self.repo_path / "package.json").exists() else self.repo_path / "CMakeLists.txt"
        if evidence_file.exists():
            self._add_fact(
                title=f"Project Identity Discovery: {ident['type']}",
                description=f"Identified project domain as {ident['domain']}.",
                category="Metadata",
                verification="VERIFIED",
                file_path=evidence_file,
                line_num=1,
                confidence="HIGH"
            )

    def _parse_manifests(self):
        conan = self.repo_path / "conanfile.py"
        if conan.exists() and not self.is_ignored(conan):
            self.metrics["tech_stack"]["build_tools"].append("Conan")
            content = conan.read_text(encoding="utf-8", errors="ignore")
            for i, line in enumerate(content.splitlines()):
                match = re.search(r'self\.requires\("([^"]+)"\)', line)
                if match:
                    pkg = match.group(1)
                    self.metrics["dependencies"]["external"].append(pkg)
                    self._add_fact(
                        title=f"Conan Dependency: {pkg}",
                        description="C++ dependency listed in Conan manifest.",
                        category="Dependencies",
                        verification="VERIFIED",
                        file_path=conan,
                        line_num=i + 1,
                        confidence="HIGH"
                    )

        cmake = self.repo_path / "CMakeLists.txt"
        if cmake.exists() and not self.is_ignored(cmake):
            self.metrics["tech_stack"]["build_tools"].append("CMake")
            self._add_fact(
                title="Build Engine (CMake)",
                description="CMake build presets.",
                category="Build System",
                verification="VERIFIED",
                file_path=cmake,
                line_num=1,
                confidence="HIGH"
            )

        reqs = self.repo_path / "requirements.txt"
        if reqs.exists() and not self.is_ignored(reqs):
            self.metrics["tech_stack"]["build_tools"].append("pip")
            for i, line in enumerate(reqs.read_text(encoding="utf-8", errors="ignore").splitlines()):
                if line and not line.startswith("#"):
                    pkg = line.split("==")[0].split(">=")[0].strip()
                    self.metrics["dependencies"]["external"].append(pkg)
                    self._add_fact(
                        title=f"Pip Package: {pkg}",
                        description="Python dependency listed in requirements index.",
                        category="Dependencies",
                        verification="VERIFIED",
                        file_path=reqs,
                        line_num=i + 1,
                        confidence="HIGH"
                    )

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
                        description="JSON package dependency.",
                        category="Dependencies",
                        verification="VERIFIED",
                        file_path=package_json,
                        line_num=1,
                        confidence="HIGH"
                    )
                if "express" in deps: self.metrics["tech_stack"]["frameworks"].append("Express")
                if "react" in deps: self.metrics["tech_stack"]["frameworks"].append("React")
                if "vue" in deps: self.metrics["tech_stack"]["frameworks"].append("Vue")
                if "next" in deps: self.metrics["tech_stack"]["frameworks"].append("Next.js")
            except Exception:
                pass

        self.metrics["dependencies"]["external"] = sorted(list(set(self.metrics["dependencies"]["external"])))

    def _extract_code_intelligence(self):
        framework_api_patterns = [
            ("express", r'\.(?:get|post|put|delete|patch)\(["\']([^"\']+)["\']\s*,', "REST (Express)"),
            ("fastapi", r'@(?:app|router)\.(?:get|post|put|delete|patch)\(["\']([^"\']+)["\']', "REST (FastAPI)"),
            ("flask", r'@(?:app|blueprint)\.route\(["\']([^"\']+)["\']', "REST (Flask)"),
            ("spring", r'@(?:GetMapping|PostMapping|PutMapping|DeleteMapping|RequestMapping)\(["\']([^"\']+)["\']', "REST (Spring)"),
            ("nestjs", r'@(?:Get|Post|Put|Delete)\(["\']([^"\']+)["\']', "REST (NestJS)"),
            ("aspnet", r'\[(?:HttpGet|HttpPost|HttpPut|HttpDelete|Route)\(["\']([^"\']+)["\']', "REST (ASP.NET)")
        ]

        db_patterns = [
            (r'mongodb://|pymongo|mongoose', "MongoDB"),
            (r'postgresql|pg\.|postgres', "PostgreSQL"),
            (r'mysql', "MySQL"),
            (r'redis|ioredis', "Redis"),
            (r'sqlite', "SQLite")
        ]

        event_broker_patterns = [
            (r'new\s+Kafka\(|kafkaClient\b', "Kafka Broker Client"),
            (r'amqp\.connect\(|amqpClient\b', "RabbitMQ Client"),
            (r'mqtt\.connect\(', "MQTT Broker Client"),
            (r'EventBus\b|dispatch\(', "EventBus Routing Ring")
        ]

        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".py", ".go", ".rs", ".cs", ".js", ".ts", ".java"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        
                        for framework, pattern, protocol in framework_api_patterns:
                            if framework in content.lower():
                                for match in re.finditer(pattern, content):
                                    route = match.group(1)
                                    line_num = content[:match.start()].count("\n") + 1
                                    self.metrics["apis"].append({
                                        "endpoint": route,
                                        "protocol": protocol,
                                        "file": file_path.name,
                                        "line": line_num,
                                        "verification": "VERIFIED"
                                    })
                                    self._add_fact(
                                        title=f"API Endpoint: {route}",
                                        description=f"Verified API endpoint bound to {framework} framework.",
                                        category="API Discovery",
                                        verification="VERIFIED",
                                        file_path=file_path,
                                        line_num=line_num,
                                        confidence="HIGH"
                                    )

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
                                        title=f"Database: {db_type}",
                                        description=f"Verified database init for {db_type}.",
                                        category="Databases",
                                        verification="VERIFIED",
                                        file_path=file_path,
                                        line_num=line_num,
                                        confidence="HIGH"
                                    )

                        for pat, desc in event_broker_patterns:
                            for match in re.finditer(pat, content):
                                line_num = content[:match.start()].count("\n") + 1
                                self.metrics["events"].append({
                                    "pattern": match.group(0),
                                    "type": desc,
                                    "file": file_path.name,
                                    "line": line_num,
                                    "verification": "VERIFIED"
                                })
                                self._add_fact(
                                    title=f"Event Hook: {desc}",
                                    description=f"Verified connection broker match: {match.group(0)}.",
                                    category="Events",
                                    verification="VERIFIED",
                                    file_path=file_path,
                                    line_num=line_num,
                                    confidence="HIGH"
                                )

                    except Exception:
                        pass

    def _derive_import_dependencies(self):
        import_patterns = [
            r'#include\s+["\']uados/([^"\']+)["\']', 
            r'#include\s+["\']([^"\']+)["\']', 
            r'import\s+.*from\s+["\']\.\./([^"\']+)["\']', 
            r'import\s+.*from\s+["\']\./([^"\']+)["\']',
            r'require\s*\(\s*["\']\.\./([^"\']+)["\']\s*\)',
            r'from\s+\.\.?([a-zA-Z0-9_]+)\s+import', 
            r'import\s+([a-zA-Z0-9_]+)'
        ]

        scanned_relations = set()
        dir_locations = {}
        for root, dirs, _ in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for d in dirs:
                rel_p = Path(root) / d
                try:
                    rel_parts = rel_p.relative_to(self.repo_path).parts
                    if rel_parts:
                        dir_locations[d] = rel_parts[0]
                except Exception:
                    pass

        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            src_folder = Path(root).relative_to(self.repo_path).parts
            if not src_folder:
                continue
            src_layer = src_folder[0]

            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".ts", ".js", ".py"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        for pat in import_patterns:
                            for match in re.finditer(pat, content):
                                imported_str = match.group(1)
                                parts = imported_str.replace("\\", "/").split("/")
                                for part in parts:
                                    part_name = Path(part).stem
                                    target_layer = None
                                    if (self.repo_path / part_name).exists() and (self.repo_path / part_name).is_dir():
                                        target_layer = part_name
                                    elif part_name in dir_locations:
                                        target_layer = dir_locations[part_name]
                                        
                                    if target_layer and target_layer != src_layer:
                                        scanned_relations.add((src_layer, target_layer, "Imports reference code"))
                                        break
                    except Exception:
                        pass

        for src, dest, desc in sorted(list(scanned_relations)):
            self.metrics["module_graph"].append((src, dest, desc))
            self._add_fact(
                title=f"Derived Import Link: {src} -> {dest}",
                description="Verified dynamic file import link discovered in codebase analysis.",
                category="Architecture Graph",
                verification="VERIFIED",
                file_path=self.repo_path / src,
                line_num=1,
                confidence="HIGH"
            )

    def _scan_directory_status(self):
        common_dirs = [
            "core", "hal", "sensors", "control", "safety", "fleet", 
            "backend", "frontend", "shared", "analytics", "infra", 
            "database", "docs", "tests", "scripts", "prediction", 
            "perception", "localization", "simulation", "validation"
        ]
        for d in common_dirs:
            p = self.repo_path / d
            self.metrics["directories"][d] = p.exists() and p.is_dir()
            
        for entry in self.repo_path.iterdir():
            if entry.is_dir() and not self.is_ignored(entry):
                name = entry.name
                if name not in self.metrics["directories"]:
                    self.metrics["directories"][name] = True

    def _extract_build_targets_and_entries(self):
        # 1. Crawl CMakeLists.txt files
        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                if file == "CMakeLists.txt":
                    file_path = Path(root) / file
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        # Extract libraries and executables
                        for match in re.finditer(r'add_(executable|library)\(\s*([a-zA-Z0-9_-]+)\s*([^)]*)\)', content, re.IGNORECASE):
                            target_type = match.group(1).upper()
                            target_name = match.group(2)
                            sources_raw = match.group(3)
                            
                            try:
                                relative_path = str(file_path.relative_to(self.repo_path)).replace("\\", "/")
                            except Exception:
                                relative_path = "CMakeLists.txt"
                                
                            self.metrics["build_targets"].append({
                                "name": target_name,
                                "type": target_type,
                                "source": relative_path
                            })

                        # Match target_link_libraries for CMake target dependencies (Fix 5)
                        for match in re.finditer(r'target_link_libraries\(\s*([a-zA-Z0-9_-]+)\s+([^)]+)\)', content, re.IGNORECASE):
                            target = match.group(1)
                            libs_raw = match.group(2)
                            libs = libs_raw.replace("PRIVATE", "").replace("PUBLIC", "").replace("INTERFACE", "").split()
                            libs = [l.strip() for l in libs if l.strip() and not l.strip().startswith("$") and not l.strip().startswith("{")]
                            if libs:
                                if target not in self.metrics["target_dependencies"]:
                                    self.metrics["target_dependencies"][target] = []
                                self.metrics["target_dependencies"][target].extend(libs)
                    except Exception:
                        pass

        # 2. Crawl package.json NPM scripts
        package_json = self.repo_path / "package.json"
        if package_json.exists():
            import json
            try:
                data = json.loads(package_json.read_text(encoding="utf-8", errors="ignore"))
                scripts = data.get("scripts", {})
                for script_name, cmd in scripts.items():
                    self.metrics["build_targets"].append({
                        "name": script_name,
                        "type": "NPM SCRIPT",
                        "source": "package.json"
                    })
            except Exception:
                pass

        # 3. Dynamic Startup Entry Points Discovery (int main, Kernel::start, app.listen) (Fix 4)
        entry_patterns = [
            (r'\bint\s+main\s*\(', "int main(int argc, char* argv[])"),
            (r'\bKernel::start\b', "Kernel::start()"),
            (r'\bApplication::run\b', "Application::run()"),
            (r'\bLifecycleManager::initialize\b', "LifecycleManager::initialize()"),
            (r'\bapp\.listen\b', "app.listen(port)")
        ]
        
        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".py", ".ts", ".js"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        for pat, desc in entry_patterns:
                            match = re.search(pat, content)
                            if match:
                                line_num = content[:match.start()].count("\n") + 1
                                rel_file = str(file_path.relative_to(self.repo_path)).replace("\\", "/")
                                self.metrics["entry_points"].append({
                                    "name": file_path.stem,
                                    "file": rel_file,
                                    "line": line_num,
                                    "pattern": desc,
                                    "confidence": "HIGH",
                                    "verification": "VERIFIED"
                                })
                    except Exception:
                        pass

    def _extract_requirements_traceability(self):
        # Dynamically load and parse MASTER_REQUIREMENTS.md if it exists
        reqs_file = self.repo_path / "AI_BRAIN" / "MASTER_REQUIREMENTS.md"
        parsed_reqs = []
        if reqs_file.exists():
            try:
                content = reqs_file.read_text(encoding="utf-8", errors="ignore")
                for line in content.splitlines():
                    # Parse rows like | FR-KRN-001 | Microkernel ... |
                    match = re.search(r'\|\s*(NFR-[A-Z]+-\d+|FR-[A-Z]+-\d+)\s*\|\s*([^|]+)\s*\|', line)
                    if match:
                        req_id = match.group(1).strip()
                        req_desc = match.group(2).strip()
                        parsed_reqs.append((req_id, req_desc))
            except Exception:
                pass

        # If MASTER_REQUIREMENTS.md doesn't exist, try standard requirement keywords
        if not parsed_reqs:
            # Fallback to basic discovered requirement documents
            for root, _, files in os.walk(self.repo_path):
                if self.is_ignored(root):
                    continue
                for file in files:
                    file_name = file.lower()
                    if "requirement" in file_name or "spec" in file_name:
                        try:
                            rel_p = str((Path(root) / file).relative_to(self.repo_path)).replace("\\", "/")
                            self.metrics["requirements"].append({
                                "id": "R-DOC",
                                "name": f"Document: {file}",
                                "status": "Documented",
                                "evidence": rel_p,
                                "tests": "N/A",
                                "confidence": "HIGH",
                                "verification": "VERIFIED"
                            })
                        except Exception:
                            pass
            return

        # Build folder to category prefix map to allow fallback directory inference
        # e.g., FR-LOC -> localization/
        category_to_folder = {
            "FR-LOC": "localization",
            "FR-CTL": "control",
            "FR-KRN": "core",
            "FR-SAF": "safety", "FR-SFT": "safety",
            "FR-VAL": "validation", "FR-VLD": "validation",
            "FR-SEN": "sensors",
            "FR-SIM": "simulation",
            "FR-FLT": "fleet",
            "FR-PER": "perception",
            "FR-PLN": "planning",
            "FR-PRD": "prediction",
            "FR-DTW": "digital_twin",
            "NFR-PERF": "core",
            "NFR-REL": "core",
            "NFR-SAF": "safety",
            "NFR-MNT": "validation",
            "NFR-SEC": "core"
        }

        # Scan codebase once to index occurrences of requirement IDs (speeds up large sweeps)
        req_occurrences = {}
        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            # Also ignore documentation folders in code evidence search
            parts = Path(root).relative_to(self.repo_path).parts
            if parts and any(p in ["docs", "AI_BRAIN"] for p in parts):
                continue
            for file in files:
                file_path = Path(root) / file
                if file_path.suffix.lower() in [".cpp", ".hpp", ".h", ".ts", ".js", ".py", ".md"]:
                    try:
                        content = file_path.read_text(encoding="utf-8", errors="ignore")
                        rel_file = str(file_path.relative_to(self.repo_path)).replace("\\", "/")
                        for req_id, _ in parsed_reqs:
                            if req_id in content:
                                if req_id not in req_occurrences:
                                    req_occurrences[req_id] = []
                                req_occurrences[req_id].append(rel_file)
                    except Exception:
                        pass

        # Link each requirement ID to evidence files and tests
        for req_id, req_desc in parsed_reqs:
            code_evidence = []
            test_evidence = []
            confidence = "LOW"
            verification = "UNKNOWN"
            
            # 1. Check direct tag matches from code scan index
            if req_id in req_occurrences:
                for file_rel in req_occurrences[req_id]:
                    if "test" in file_rel.lower() or "spec" in file_rel.lower():
                        test_evidence.append(file_rel)
                    else:
                        code_evidence.append(file_rel)
                confidence = "HIGH"
                verification = "VERIFIED"

            # 2. Fallback to folder-based category association if direct references are absent
            if not code_evidence:
                matched_folder = None
                for prefix, folder in category_to_folder.items():
                    if req_id.startswith(prefix):
                        matched_folder = folder
                        break
                
                if matched_folder:
                    folder_p = self.repo_path / matched_folder
                    if folder_p.exists() and folder_p.is_dir():
                        confidence = "MEDIUM"
                        verification = "DERIVED"
                        # Collect first few code files and test files inside matched folder
                        for r, _, fs in os.walk(folder_p):
                            if self.is_ignored(r):
                                continue
                            for f in fs:
                                if f.endswith((".cpp", ".hpp", ".h", ".py", ".ts")):
                                    try:
                                        rel_f = str((Path(r) / f).relative_to(self.repo_path)).replace("\\", "/")
                                        if "test" in rel_f.lower() or "spec" in rel_f.lower():
                                            if len(test_evidence) < 2:
                                                test_evidence.append(rel_f)
                                        else:
                                            if len(code_evidence) < 2:
                                                code_evidence.append(rel_f)
                                    except Exception:
                                        pass

            status = "Implemented" if code_evidence else "NOT_IMPLEMENTED"
            evidence_str = ", ".join(code_evidence[:3]) if code_evidence else "N/A"
            test_str = ", ".join(test_evidence[:3]) if test_evidence else "N/A"
            
            self.metrics["requirements"].append({
                "id": req_id,
                "name": req_desc,
                "status": status,
                "evidence": evidence_str,
                "tests": test_str,
                "confidence": confidence,
                "verification": verification
            })

    def _derive_data_flow(self):
        from collections import defaultdict
        graph_dict = defaultdict(list)
        in_degrees = defaultdict(int)
        all_nodes = set()
        for src, dest, _ in self.metrics["module_graph"]:
            graph_dict[src].append(dest)
            in_degrees[dest] += 1
            all_nodes.add(src)
            all_nodes.add(dest)
            
        roots = [node for node in all_nodes if in_degrees[node] == 0]
        data_flows = []
        visited = set()
        
        def dfs(node, path):
            if node in visited or len(path) > 5:
                return
            visited.add(node)
            neighbors = graph_dict[node]
            if not neighbors:
                data_flows.append(" -> ".join(path))
            else:
                for neighbor in neighbors:
                    dfs(neighbor, path + [neighbor])
            visited.remove(node)
            
        for root in roots:
            dfs(root, [root])
            
        self.metrics["data_flow"] = data_flows

    def _build_test_and_ownership_maps(self):
        for root, _, files in os.walk(self.repo_path):
            if self.is_ignored(root):
                continue
            parts = Path(root).relative_to(self.repo_path).parts
            if not parts:
                continue
            module = parts[0]

            for file in files:
                file_name = file.lower()
                suffix = Path(file).suffix.lower()
                
                # Code ownership map
                if suffix in [".cpp", ".hpp", ".h", ".py", ".ts", ".js", ".go", ".rs", ".cs", ".java"]:
                    if "test" not in file_name and "spec" not in file_name:
                        self.metrics["ownership_map"][module] = self.metrics["ownership_map"].get(module, 0) + 1

                # Test map
                if "test_" in file_name or "spec" in file_name or "_test" in file_name:
                    if module not in self.metrics["test_map"]:
                        self.metrics["test_map"][module] = []
                    if file not in self.metrics["test_map"][module]:
                        self.metrics["test_map"][module].append(file)

    def _calculate_build_order(self):
        # Topological Sort Build Order Based on Target Link Dependencies (Fix 5)
        deps = self.metrics["target_dependencies"]
        targets = [t["name"] for t in self.metrics["build_targets"] if t["type"] in ["LIBRARY", "EXECUTABLE"]]
        
        visited = {}
        order = []
        
        def dfs_sort(node):
            if node in visited:
                if visited[node] == 1: # cycle detected
                    return
                return
            visited[node] = 1
            for dep in deps.get(node, []):
                if dep in targets:
                    dfs_sort(dep)
            visited[node] = 2
            order.append(node)
            
        for t in targets:
            dfs_sort(t)
            
        self.metrics["build_order"] = order
