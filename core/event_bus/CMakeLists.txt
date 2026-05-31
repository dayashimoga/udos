# uados_event_bus
add_library(uados_event_bus STATIC
    src/event_bus_impl.cpp
)
add_library(uados::event_bus ALIAS uados_event_bus)

target_include_directories(uados_event_bus PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_event_bus PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
