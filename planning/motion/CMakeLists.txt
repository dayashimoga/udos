# uados_plan_motion
add_library(uados_plan_motion STATIC
    src/motion_planner.cpp
)
add_library(uados::plan_motion ALIAS uados_plan_motion)

target_include_directories(uados_plan_motion PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_plan_motion PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
