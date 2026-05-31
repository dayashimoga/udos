#!/usr/bin/env bash
# ==============================================================================
# UADOS — Cross-Platform Build Helper
# ==============================================================================
# Performs developer build orchestration using Conan package manager and CMake.
#
# Usage:
#   ./scripts/build/build.sh [options]
#
# Options:
#   --clean          Remove build artifacts before compiling
#   --debug          Configure for Debug instead of Release
#   --no-tests       Disable compiling test suites
#   --help           Show this usage guide
# ==============================================================================

set -euo pipefail

# Resolve script root and workdir
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "${WORK_DIR}"

BUILD_TYPE="Release"
BUILD_TESTS="True"
CLEAN_BUILD="False"

# Command line parsing
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD="True"
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --no-tests)
            BUILD_TESTS="False"
            shift
            ;;
        --help)
            echo "Usage: ./build.sh [options]"
            echo ""
            echo "Options:"
            echo "  --clean       Clean build directories before running"
            echo "  --debug       Compile with debug symbols"
            echo "  --no-tests    Skip compiling GTest test suites"
            echo "  --help        Print this instructions dialog"
            exit 0
            ;;
        *)
            echo "Error: Unknown argument '$1'"
            exit 1
            ;;
    esac
done

echo "====== UADOS Build System Initializing ======"
echo "Workspace Directory: ${WORK_DIR}"
echo "Build Mode:          ${BUILD_TYPE}"
echo "Compile Test Suites: ${BUILD_TESTS}"
echo "============================================="

# 1. Clean build directory if requested
if [ "${CLEAN_BUILD}" = "True" ]; then
    echo "[Build] Cleaning previous build folders..."
    rm -rf build
fi

# 2. Run Conan dependency installation
echo "[Build] Invoking Conan package dependency install..."
conan install . \
    --output-folder=build \
    --build=missing \
    -s build_type="${BUILD_TYPE}" \
    -o build_tests="${BUILD_TESTS}"

# 3. Configure via CMake
echo "[Build] Invoking CMake generator..."
cmake --preset conan-"$(echo "${BUILD_TYPE}" | tr '[:upper:]' '[:lower:]')"

# 4. Compile targets
echo "[Build] Compiling C++ targets..."
cmake --build --preset conan-"$(echo "${BUILD_TYPE}" | tr '[:upper:]' '[:lower:]')"

echo "====== UADOS Build Successfully Completed! ======"
