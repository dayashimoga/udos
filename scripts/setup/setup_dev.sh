#!/usr/bin/env bash
# ==============================================================================
# UADOS — Developer Environment Bootstrap Setup
# ==============================================================================
# Prepares developer host machine with Conan profiles, python requirements,
# and developer toolchains required for local compilation.
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "${WORK_DIR}"

echo "====== UADOS Onboarding Environment Bootstrapper ======"
echo "Workspace Directory: ${WORK_DIR}"
echo "======================================================="

# 1. Detect Python3 installation
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is required but could not be located in current PATH."
    exit 1
fi

# 2. Establish python virtual environment
echo "[Setup] Preparing Python virtual environment..."
if [ ! -d ".venv" ]; then
    python3 -m venv .venv
fi

# Activate virtual environment
source .venv/bin/activate

# 3. Upgrade Pip & Install requirements
echo "[Setup] Installing pip dependencies (conan, black, pytest, cppcheck)..."
pip install --upgrade pip
pip install conan black pytest numpy matplotlib pandas nlohmann-json-schema

# 4. Check for Conan configuration and setup default profile
echo "[Setup] Checking Conan package manager configuration..."
if ! command -v conan &> /dev/null; then
    # Conan might be in venv bin
    export PATH="${WORK_DIR}/.venv/bin:${PATH}"
fi

# Create default conan profile if not exists
if ! conan profile show default &> /dev/null; then
    echo "[Setup] Creating default Conan environment profiles..."
    conan profile detect --force
fi

echo "====== UADOS Bootstrap Setup Successfully Completed! ======"
echo "To activate your virtual environment, run: source .venv/bin/activate"
