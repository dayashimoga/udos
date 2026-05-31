# Universal AI Project Brain Framework (AIPBF) — Usage Guide

AIPBF is a **zero-dependency, repository-agnostic** static analysis tool. It can be integrated into *any* codebase to generate comprehensive documentation, security audits, and AI context packages automatically.

---

## 1. Local execution (Zero Dependencies)

AIPBF has **no external library dependencies** (it uses standard Python modules: `os`, `sys`, `pathlib`, `re`, `argparse`, `datetime`, `json`). You only need Python 3.8+ installed on your host machine.

### Installation in a target repository:
1. Copy the files in `tools/project_brain/` to your project's directory.
2. Run the onboarding bootstrapper:
   - **Linux/macOS**: `./scripts/install-project-brain`
   - **Windows**: `.\scripts\install-project-brain.bat`

### CLI commands:
- **Scan and sync documents**:
  ```bash
  python tools/project_brain/project_brain.py --scan
  ```
- **Run quality & security audit**:
  ```bash
  python tools/project_brain/project_brain.py --review
  ```
- **Target a specific path**:
  ```bash
  python tools/project_brain/project_brain.py --scan --path /path/to/other/repo
  ```

---

## 2. Docker Execution (Containerized)

If you **do not want to install Python locally**, you can run the framework completely containerized via Docker.

### Step 1: Build the Docker Image
From the `tools/project_brain/` directory (where the `Dockerfile` resides), run:
```bash
docker build -t project-brain .
```

### Step 2: Run in any repository
To scan any new or existing repository, mount your project directory into the container's `/workspace` volume and execute:

#### On Linux / macOS:
```bash
docker run --rm -v "$(pwd):/workspace" project-brain --scan --path /workspace
```

#### On Windows (PowerShell):
```powershell
docker run --rm -v "${PWD}:/workspace" project-brain --scan --path /workspace
```

#### On Windows (Command Prompt):
```cmd
docker run --rm -v "%cd%:/workspace" project-brain --scan --path /workspace
```

### Review Heuristic Audit findings via Docker:
```bash
docker run --rm -v "$(pwd):/workspace" project-brain --review --path /workspace
```

---

## 3. Automation integration

### Git pre-commit hook:
The installer automatically configures a git pre-commit hook. Every time you run `git commit`, the brain will automatically run a scan and stage the updated `PROJECT_BRAIN.md`, `CODE_UNDERSTANDING.md`, and `AI_CONTEXT_PACKAGE.md` files.

### CI/CD Pipelines:
Simply run the docker command in your CI pipeline runner (e.g. GitHub Actions, GitLab CI) to check for undocumented API additions, security alerts, or complexity index degradations before accepting pull requests!
