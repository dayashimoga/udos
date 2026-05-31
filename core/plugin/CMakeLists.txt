# uados_plugin
add_library(uados_plugin STATIC
    src/plugin_system_impl.cpp
)
add_library(uados::plugin ALIAS uados_plugin)

target_include_directories(uados_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_plugin PUBLIC
    uados::common
    uados_options
    ${CMAKE_DL_LIBS}
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
