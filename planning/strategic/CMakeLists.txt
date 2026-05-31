# uados_plan_strategic
add_library(uados_plan_strategic STATIC
    src/strategic_planner.cpp
)
add_library(uados::plan_strategic ALIAS uados_plan_strategic)

target_include_directories(uados_plan_strategic PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_plan_strategic PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
