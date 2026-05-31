# uados_localization_pose
add_library(uados_localization_pose STATIC
    src/pose_estimator.cpp
)
add_library(uados::localization_pose ALIAS uados_localization_pose)

target_include_directories(uados_localization_pose PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(uados_localization_pose PUBLIC
    uados::common
    uados_options
)

if(UADOS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
