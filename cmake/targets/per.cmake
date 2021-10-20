cmake_minimum_required(VERSION 3.16)

set(LIBRARY_SOURCES
        per.cpp
        )
list(TRANSFORM LIBRARY_SOURCES PREPEND "${PROJECT_PER_SRC_DIR}/")

add_library(${per_lib}_interface INTERFACE)
add_library(${per_lib}_static STATIC)
add_library(${per_lib}_shared SHARED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

target_sources(${per_lib}_static PRIVATE ${LIBRARY_SOURCES})
target_sources(${per_lib}_shared PRIVATE ${LIBRARY_SOURCES})

target_include_directories(${per_lib}_interface
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_PER_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
        )

target_link_libraries(${per_lib}_interface
        INTERFACE
        project_options
        pybind11::module
        $<$<NOT:$<BOOL:USE_PYBIND11_FINDPYTHON>>:Python3::Module>
        )

target_link_libraries(${per_lib}_static
        PRIVATE
        project_warnings
        PUBLIC
        ${per_lib}_interface
        )

target_link_libraries(${per_lib}_shared
        PRIVATE
        project_warnings
        PUBLIC
        ${per_lib}_interface
        )
set_target_properties(${per_lib}_shared
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        )