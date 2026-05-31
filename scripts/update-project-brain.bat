@echo off
rem ==============================================================================
rem UADOS — Project Brain Sync (Windows Batch)
rem ==============================================================================

echo [Auto-Sync] Running Project Brain Sync (Windows)...
python "%~dp0..\tools\analysis\doc_generator.py"
echo [Auto-Sync] Project Brain Sync complete!
