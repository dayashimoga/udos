# uados_plan_behavior
add_library(uados_plan_behavior STATIC
    src/behavior_planner.cpp
)
add_library(uados::plan_behavior ALIAS uados_plan_behavior)

target_include_directories(uados_plan_behavior PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_plan_behavior PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
