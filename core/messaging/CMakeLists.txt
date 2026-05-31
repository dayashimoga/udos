# uados_messaging
add_library(uados_messaging INTERFACE)
add_library(uados::messaging ALIAS uados_messaging)
target_include_directories(uados_messaging INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(uados_messaging INTERFACE
    uados::common;uados_event_bus
    uados_options
)
if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
