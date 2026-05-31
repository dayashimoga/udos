# uados_scheduler
add_library(uados_scheduler STATIC
    src/scheduler_impl.cpp
)
add_library(uados::scheduler ALIAS uados_scheduler)

target_include_directories(uados_scheduler PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_scheduler PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
