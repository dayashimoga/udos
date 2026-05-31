# ============================================================================
# UADOS — Common Library
# Shared types, utilities, and base classes used across all modules
# ============================================================================

add_library(uados_common STATIC
    src/resource_profiler.cpp
)
add_library(uados::common ALIAS uados_common)

target_include_directories(uados_common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_common PUBLIC
    fmt::fmt
    spdlog::spdlog
    Eigen3::Eigen
    uados_options
)

# Tests
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
