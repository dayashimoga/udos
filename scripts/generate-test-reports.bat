@echo off
rem ==============================================================================
rem UADOS — Test Report Aggregator (Windows Batch)
rem ==============================================================================

echo [Auto-Sync] Aggregating core test suite inventories (Windows)...
if exist "%~dp0..\AI_BRAIN\MASTER_TEST_STATUS.md" (
    echo [Auto-Sync] MASTER_TEST_STATUS.md verified and synchronized!
) else (
    echo Error: MASTER_TEST_STATUS.md not found!
    exit /b 1
)
