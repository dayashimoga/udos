@echo off
rem ==============================================================================
rem UADOS — Developer Environment Setup (Windows)
rem ==============================================================================

echo ====== UADOS Onboarding Environment Bootstrapper (Windows) ======

where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: Python is required but could not be located in PATH!
    exit /b 1
)

echo [Setup] Preparing Python virtual environment...
if not exist .venv (
    python -m venv .venv
)

call .venv\Scripts\activate.bat
if %ERRORLEVEL% neq 0 (
    echo Error: Virtualenv activation failed!
    exit /b %ERRORLEVEL%
)

echo [Setup] Installing pip dependencies (conan, black, pytest, numpy, matplotlib)...
python -m pip install --upgrade pip
pip install conan black pytest numpy matplotlib pandas

echo [Setup] Checking Conan package manager configuration...
conan profile show default >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [Setup] Creating default Conan profiles...
    conan profile detect --force
)

echo ====== UADOS Bootstrap Setup Successfully Completed! ======
echo To activate your virtual environment, run: .venv\Scripts\activate.bat
exit /b 0
