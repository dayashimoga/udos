@echo off
rem ==============================================================================
rem UADOS — Windows Build Helper Batch Script
rem ==============================================================================

set BUILD_TYPE=Release
set BUILD_TESTS=True
set CLEAN_BUILD=False

:parse_args
if "%1"=="" goto run_build
if "%1"=="--clean" (
    set CLEAN_BUILD=True
    shift
    goto parse_args
)
if "%1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto parse_args
)
if "%1"=="--no-tests" (
    set BUILD_TESTS=False
    shift
    goto parse_args
)
if "%1"=="--help" (
    echo Usage: build.bat [options]
    echo.
    echo Options:
    echo   --clean       Clean build directories before running
    echo   --debug       Compile with debug symbols
    echo   --no-tests    Skip compiling GTest test suites
    echo   --help        Print this instructions dialog
    exit /b 0
)
echo Error: Unknown argument "%1"
exit /b 1

:run_build
echo ====== UADOS Build System Initializing (Windows) ======
echo Build Mode:          %BUILD_TYPE%
echo Compile Test Suites: %BUILD_TESTS%
echo =======================================================

if "%CLEAN_BUILD%"=="True" (
    echo [Build] Cleaning previous build folders...
    if exist build rmdir /s /q build
)

echo [Build] Invoking Conan package dependency install...
conan install . --output-folder=build --build=missing -s build_type=%BUILD_TYPE% -o build_tests=%BUILD_TESTS%
if %ERRORLEVEL% neq 0 (
    echo Error: Conan install failed!
    exit /b %ERRORLEVEL%
)

echo [Build] Invoking CMake generator...
set PRESET_NAME=conan-release
if "%BUILD_TYPE%"=="Debug" (
    set PRESET_NAME=conan-debug
)

cmake --preset %PRESET_NAME%
if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed!
    exit /b %ERRORLEVEL%
)

echo [Build] Compiling C++ targets...
cmake --build --preset %PRESET_NAME%
if %ERRORLEVEL% neq 0 (
    echo Error: Compilation failed!
    exit /b %ERRORLEVEL%
)

echo ====== UADOS Build Successfully Completed! ======
exit /b 0
