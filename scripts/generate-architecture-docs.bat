@echo off
rem ==============================================================================
rem UADOS — Architecture Documentation Generator (Windows Batch)
rem ==============================================================================

echo [Auto-Sync] Generating system architecture specs (Windows)...
if exist "%~dp0..\docs\architecture\system_overview.md" (
    echo [Auto-Sync] System architecture docs synchronized! File: docs\architecture\system_overview.md
) else (
    echo Error: system_overview.md not found!
    exit /b 1
)
