# uados_lifecycle
add_library(uados_lifecycle STATIC
    src/lifecycle_manager_impl.cpp
)
add_library(uados::lifecycle ALIAS uados_lifecycle)

target_include_directories(uados_lifecycle PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_lifecycle PUBLIC
    uados::common
    uados::health
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
