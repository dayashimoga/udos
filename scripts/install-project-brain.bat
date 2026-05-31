@echo off
rem ==============================================================================
rem Universal AI Project Brain Framework (AIPBF) Onboarding (Windows Batch)
rem ==============================================================================

echo ====== AIPBF Installer Bootstrapping (Windows) ======
echo Target Workspace: %~dp0..
echo ======================================================

where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: Python is required but could not be located in PATH!
    exit /b 1
)

echo [Install] Running initial repository crawling analysis...
python "%~dp0..\tools\project_brain\project_brain.py" --init --scan --path "%~dp0.."

if exist "%~dp0..\.git" (
    echo [Install] Injecting automated Git pre-commit hook...
    (
    echo #!/usr/bin/env bash
    echo python tools/project_brain/project_brain.py --scan
    echo git add AI_BRAIN/PROJECT_BRAIN.md CODE_UNDERSTANDING.md AI_CONTEXT_PACKAGE.md
    ) > "%~dp0..\.git\hooks\pre-commit"
    echo [Install] Pre-commit hook successfully established!
)

echo ====== AIPBF Onboarding Completed Successfully! ======
echo Master document generated at: \AI_BRAIN\PROJECT_BRAIN.md
exit /b 0
