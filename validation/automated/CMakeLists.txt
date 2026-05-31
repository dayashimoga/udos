# uados_val_automated
add_library(uados_val_automated STATIC
    src/automated_validator.cpp
)
add_library(uados::val_automated ALIAS uados_val_automated)

target_include_directories(uados_val_automated PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_val_automated PUBLIC
    uados::common
    uados_options
    uados::sim_scenarios
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
