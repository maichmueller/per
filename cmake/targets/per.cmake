
set(LIBRARY_SOURCES
        arbitrary.cpp
        )
list(TRANSFORM LIBRARY_SOURCES PREPEND "${PROJECT_PER_SRC_DIR}/")

add_library(${per_lib} ${per-lib-type})

target_sources(
        ${per_lib}
        PRIVATE
        ${LIBRARY_SOURCES}
)

target_include_directories(
        ${per_lib}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_PER_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
        ${per_lib}
        PRIVATE
        project_warnings
        PUBLIC
        project_options
        pybind11::module
        $<$<NOT:$<BOOL:USE_PYBIND11_FINDPYTHON>>:Python3::Module>
)

set_target_properties(
        ${per_lib}
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        $<$<PLATFORM_ID:WIN32>:MSVC_RUNTIME_LIBRARY ${CMAKE_MSVC_RUNTIME_LIBRARY}>
)

target_compile_options(
        ${per_lib}
        PRIVATE
        $<$<AND:$<PLATFORM_ID:WIN32>,${msvc-rt-mtd-static}>:-MTd>
        $<$<AND:$<PLATFORM_ID:WIN32>,${msvc-rt-mt-static}>:-MT>
        $<$<AND:$<PLATFORM_ID:WIN32>,${msvc-rt-mtd-dll}>:-MDd>
        $<$<AND:$<PLATFORM_ID:WIN32>,${msvc-rt-mt-dll}>:-MD>
)