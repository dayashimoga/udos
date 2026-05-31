# uados_safety_emergency
add_library(uados_safety_emergency STATIC
    src/emergency_response_system.cpp
)
add_library(uados::safety_emergency ALIAS uados_safety_emergency)

target_include_directories(uados_safety_emergency PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_safety_emergency PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
