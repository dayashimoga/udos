# uados_val_fault_injection
add_library(uados_val_fault_injection STATIC
    src/fault_injector.cpp
)
add_library(uados::val_fault_injection ALIAS uados_val_fault_injection)

target_include_directories(uados_val_fault_injection PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_val_fault_injection PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
