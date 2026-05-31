cmake_minimum_required(VERSION 3.28)

# ============================================================================
# UADOS — Universal Autonomous Driving Operating System
# Root CMake Configuration
# ============================================================================

project(uados
    VERSION 0.1.0
    DESCRIPTION "Universal Autonomous Driving Operating System"
    HOMEPAGE_URL "https://github.com/uados/uados"
    LANGUAGES CXX C
)

# ----------------------------------------------------------------------------
# Global C++ Standard
# ----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------------------------------
# Build Options
# ----------------------------------------------------------------------------
option(UADOS_BUILD_TESTS "Build unit and integration tests" ON)
option(UADOS_BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(UADOS_BUILD_DOCS "Build documentation" OFF)
option(UADOS_BUILD_EXAMPLES "Build examples" OFF)
option(UADOS_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)
option(UADOS_ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)
option(UADOS_BUILD_PERCEPTION "Build perception module (requires ONNX Runtime)" OFF)
option(UADOS_BUILD_SIMULATION "Build simulation drivers (requires CARLA)" OFF)
option(UADOS_BUILD_RC_CAR "Build RC car drivers" OFF)
option(UADOS_BUILD_FLEET "Build fleet platform" OFF)

# ----------------------------------------------------------------------------
# Build Type Default
# ----------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# ----------------------------------------------------------------------------
# Compiler Warnings
# ----------------------------------------------------------------------------
add_library(uados_warnings INTERFACE)
target_compile_options(uados_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
        -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion -Wnull-dereference
        -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
        -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion -Wnull-dereference
        -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /permissive- /w14640 /w14242 /w14254 /w14263
        /w14265 /w14287 /w14296 /w14311 /w14545 /w14546
        /w14547 /w14549 /w14555 /w14619 /w14826 /w14905
        /w14906 /w14928
    >
)

# ----------------------------------------------------------------------------
# Sanitizers
# ----------------------------------------------------------------------------
add_library(uados_sanitizers INTERFACE)
if(UADOS_ENABLE_SANITIZERS)
    target_compile_options(uados_sanitizers INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(uados_sanitizers INTERFACE
        -fsanitize=address,undefined
    )
endif()

# ----------------------------------------------------------------------------
# Coverage
# ----------------------------------------------------------------------------
add_library(uados_coverage INTERFACE)
if(UADOS_ENABLE_COVERAGE)
    target_compile_options(uados_coverage INTERFACE --coverage -O0 -g)
    target_link_options(uados_coverage INTERFACE --coverage)
endif()

# ----------------------------------------------------------------------------
# Global Compile Options Interface
# ----------------------------------------------------------------------------
add_library(uados_options INTERFACE)
target_link_libraries(uados_options INTERFACE
    uados_warnings
    uados_sanitizers
    uados_coverage
)

# ----------------------------------------------------------------------------
# Find Dependencies (Conan-managed or system)
# ----------------------------------------------------------------------------
# Core dependencies (always required)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# Math/science (required for most modules)
find_package(Eigen3 CONFIG REQUIRED)

# Testing
if(UADOS_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)
endif()

# Benchmarks
if(UADOS_BUILD_BENCHMARKS)
    find_package(benchmark CONFIG REQUIRED)
endif()

# ----------------------------------------------------------------------------
# UADOS Common Library
# ----------------------------------------------------------------------------
add_subdirectory(core/common)

# ----------------------------------------------------------------------------
# Core Kernel Components
# ----------------------------------------------------------------------------
add_subdirectory(core/kernel)
add_subdirectory(core/event_bus)
add_subdirectory(core/scheduler)
add_subdirectory(core/health)
add_subdirectory(core/lifecycle)
add_subdirectory(core/plugin)
add_subdirectory(core/messaging)

# ----------------------------------------------------------------------------
# Vehicle Abstraction Layer
# ----------------------------------------------------------------------------
add_subdirectory(hal/api)
add_subdirectory(hal/sdk)
add_subdirectory(hal/drivers/simulation)

if(UADOS_BUILD_RC_CAR)
    add_subdirectory(hal/drivers/rc_car)
endif()

add_subdirectory(hal/drivers/canbus)
add_subdirectory(hal/validation)

# ----------------------------------------------------------------------------
# Sensor Platform
# ----------------------------------------------------------------------------
add_subdirectory(sensors/api)
add_subdirectory(sensors/camera)
add_subdirectory(sensors/radar)
add_subdirectory(sensors/lidar)
add_subdirectory(sensors/gps)
add_subdirectory(sensors/imu)
add_subdirectory(sensors/fusion)

# ----------------------------------------------------------------------------
# Perception (requires ML runtime)
# ----------------------------------------------------------------------------
if(UADOS_BUILD_PERCEPTION)
    add_subdirectory(perception/detection)
    add_subdirectory(perception/classification)
    add_subdirectory(perception/tracking)
    add_subdirectory(perception/segmentation)
    add_subdirectory(perception/lanes)
    add_subdirectory(perception/signs)
    add_subdirectory(perception/traffic_lights)
endif()

# ----------------------------------------------------------------------------
# Localization
# ----------------------------------------------------------------------------
add_subdirectory(localization/gps_fusion)
add_subdirectory(localization/visual)
add_subdirectory(localization/slam)
add_subdirectory(localization/hdmap)
add_subdirectory(localization/pose)

# ----------------------------------------------------------------------------
# Prediction
# ----------------------------------------------------------------------------
add_subdirectory(prediction/trajectory)
add_subdirectory(prediction/behavior)
add_subdirectory(prediction/risk)

# ----------------------------------------------------------------------------
# Planning
# ----------------------------------------------------------------------------
add_subdirectory(planning/strategic)
add_subdirectory(planning/behavior)
add_subdirectory(planning/motion)

# ----------------------------------------------------------------------------
# Control
# ----------------------------------------------------------------------------
add_subdirectory(control/steering)
add_subdirectory(control/brake)
add_subdirectory(control/throttle)
add_subdirectory(control/transmission)
add_subdirectory(control/loops)

# ----------------------------------------------------------------------------
# Safety Platform
# ----------------------------------------------------------------------------
add_subdirectory(safety/monitors)
add_subdirectory(safety/fault_detection)
add_subdirectory(safety/runtime_validation)
add_subdirectory(safety/emergency)

# ----------------------------------------------------------------------------
# Digital Twin Platform
# ----------------------------------------------------------------------------
add_subdirectory(digital_twin/vehicle)
add_subdirectory(digital_twin/sensor)

# ----------------------------------------------------------------------------
# Simulation Platform
# ----------------------------------------------------------------------------
add_subdirectory(simulation/scenarios)
add_subdirectory(simulation/replay)

# ----------------------------------------------------------------------------
# Validation Platform
# ----------------------------------------------------------------------------
add_subdirectory(validation/automated)
add_subdirectory(validation/fault_injection)

# ----------------------------------------------------------------------------
# Fleet Platform
# ----------------------------------------------------------------------------
add_subdirectory(fleet/telemetry)
add_subdirectory(fleet/ota)

# ----------------------------------------------------------------------------
# Print Configuration Summary
# ----------------------------------------------------------------------------
message(STATUS "")
message(STATUS "╔══════════════════════════════════════════════════════════╗")
message(STATUS "║  UADOS v${PROJECT_VERSION} Configuration Summary          ║")
message(STATUS "╠══════════════════════════════════════════════════════════╣")
message(STATUS "║  Build Type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "║  C++ Standard:    C++${CMAKE_CXX_STANDARD}")
message(STATUS "║  Compiler:        ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "║  Tests:           ${UADOS_BUILD_TESTS}")
message(STATUS "║  Benchmarks:      ${UADOS_BUILD_BENCHMARKS}")
message(STATUS "║  Sanitizers:      ${UADOS_ENABLE_SANITIZERS}")
message(STATUS "║  Coverage:        ${UADOS_ENABLE_COVERAGE}")
message(STATUS "║  Perception:      ${UADOS_BUILD_PERCEPTION}")
message(STATUS "║  Simulation:      ${UADOS_BUILD_SIMULATION}")
message(STATUS "║  RC Car:          ${UADOS_BUILD_RC_CAR}")
message(STATUS "║  Fleet:           ${UADOS_BUILD_FLEET}")
message(STATUS "╚══════════════════════════════════════════════════════════╝")
message(STATUS "")
