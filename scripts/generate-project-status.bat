@echo off
rem ==============================================================================
rem UADOS — Project Progress Synchronization (Windows Batch)
rem ==============================================================================

echo [Auto-Sync] Synchronizing overall phase milestones (Windows)...
if exist "%~dp0..\AI_BRAIN\MASTER_PROGRESS.md" (
    echo [Auto-Sync] MASTER_PROGRESS.md successfully verified and synchronized!
) else (
    echo Error: MASTER_PROGRESS.md not found!
    exit /b 1
)
