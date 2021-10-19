cmake_minimum_required(VERSION 3.16)

set(LIBRARY_SOURCES
        per.cpp
        )
list(TRANSFORM LIBRARY_SOURCES PREPEND "${PROJECT_PER_SRC_DIR}/")

add_library(${per_lib_name}_interface INTERFACE)
add_library(${per_lib_name}_static STATIC)
add_library(${per_lib_name}_shared SHARED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

target_sources(${per_lib_name}_static PRIVATE ${LIBRARY_SOURCES})
target_sources(${per_lib_name}_shared PRIVATE ${LIBRARY_SOURCES})

target_include_directories(${per_lib_name}_interface
        INTERFACE
        ${Python_INCLUDE_DIRS}
        $<BUILD_INTERFACE:${PROJECT_PER_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
        )

target_link_libraries(${per_lib_name}_interface
        INTERFACE
        project_options
        CONAN_PKG::pybind11
        ${Python_LIBRARIES}
        )

target_link_libraries(${per_lib_name}_static
        PRIVATE
        project_warnings
        PUBLIC
        ${per_lib_name}_interface
        )

target_link_libraries(${per_lib_name}_shared
        PRIVATE
        project_warnings
        PUBLIC
        ${per_lib_name}_interface
        )
set_target_properties(${per_lib_name}_shared
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        )