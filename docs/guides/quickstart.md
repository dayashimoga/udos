# UADOS — Developer Quickstart Guide

Welcome to the Universal Autonomous Driving Operating System (UADOS)! Follow this guide to configure your local developer workstation, build the codebase, run test suites, and launch the digital twin visualizer dashboard.

---

## 1. Prerequisites

Ensure your system has the following installed:
- **C++ Compiler**: GCC 11+, Clang 13+, or Visual Studio 2022
- **CMake**: version 3.24 or higher
- **Python**: version 3.9 or higher (with pip)
- **Conan**: package manager version 2.0+

---

## 2. Environment Bootstrap Setup

Run the bootstrapper to configure python dependencies, Conan profiles, and virtual environment setup:

### Windows (PowerShell/CMD):
```powershell
.\scripts\setup\setup_dev.bat
```

### macOS / Linux:
```bash
chmod +x ./scripts/setup/setup_dev.sh
./scripts/setup/setup_dev.sh
```

Ensure to activate the virtual environment once configured:
- **Windows**: `.venv\Scripts\activate.bat`
- **Linux/macOS**: `source .venv/bin/activate`

---

## 3. Build & Compilation

To compile all core packages, tests, and mock platforms, run the build helper script:

### Windows:
```powershell
.\scripts\build\build.bat
```

### Linux/macOS:
```bash
chmod +x ./scripts/build/build.sh
./scripts/build/build.sh
```

---

## 4. Run Test Suites

Verify implementation correctness by executing the Google Test validation suite:

```bash
# Navigate to the compiled build directory
cd build

# Execute all test targets
ctest --output-on-failure
```

---

## 5. Visual Dashboard Execution

The digital twin visual dashboard allows you to view system status, trajectories, sensor streams, and safety metrics inside any web browser.

1. Navigate to the dashboard directory:
   ```bash
   cd digital_twin/dashboard/
   ```
2. Start a simple local python web server:
   ```bash
   python -m http.server 8000
   ```
3. Open your browser and navigate to: [http://localhost:8000](http://localhost:8000)

Now you can interact with the dynamic physics simulation, trigger fault injections, and watch the Stanley controller adapt real-time!
