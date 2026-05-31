@echo off
rem ==============================================================================
rem UADOS — Code Understanding Engine Generator (Windows Batch)
rem ==============================================================================

echo [Auto-Sync] Initiating Code Understanding Engine crawl (Windows)...
python "%~dp0..\tools\analysis/doc_generator.py"
echo [Auto-Sync] CODE_UNDERSTANDING.md successfully updated!
