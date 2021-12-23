
add_library(${per_lib} ${per-lib-type})

target_include_directories(
        ${per_lib}
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_PER_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
        ${per_lib}
        INTERFACE
        project_options
        CONAN_PKG::cppitertools
)

set_target_properties(
        ${per_lib}
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
)