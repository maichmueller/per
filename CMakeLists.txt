cmake_minimum_required(VERSION 3.16)

project(PER VERSION 0.1 LANGUAGES CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)


# Use C++17
set(PROJ_CXX_STD_FEATURE cxx_std_17)
set(PROJ_CXX_STANDARD C++17)

set(per_lib per++)
set(per_pymodule pyper)
set(per_test tests)
set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(PROJECT_PER_DIR "${PROJECT_SRC_DIR}/per")
set(PROJECT_PYPER_DIR "${PROJECT_SRC_DIR}/pyper")
set(PROJECT_PER_SRC_DIR "${PROJECT_PER_DIR}/impl")
set(PROJECT_PER_INCLUDE_DIR "${PROJECT_PER_DIR}/include")
set(PROJECT_PER_BINDING_DIR "${PROJECT_PER_DIR}/binding")
set(CMAKE_CONFIG_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# options

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_BUILD_DOCS "Enable building the docs. Requires doxygen to be installed on the system" OFF)
option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
option(ENABLE_CACHE "Enable cache if available" ON)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)
option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)
option(ENABLE_PCH "Enable Precompiled Headers" ON)
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(USE_PYBIND11_FINDPYTHON "Use pybind11 to search for the python library" OFF)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

message(STATUS "PER project files directory: ${PROJECT_SRC_DIR}")
message(STATUS "PER src directory: ${PROJECT_PER_SRC_DIR}")
message(STATUS "PER include directory: ${PROJECT_PER_INCLUDE_DIR}")
message(STATUS "PER bindings directory: ${PROJECT_PER_BINDING_DIR}")

message("C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("C++ Compiler Info: ${CMAKE_CXX_COMPILER_ID}, version ${CMAKE_CXX_COMPILER_VERSION}")
message("CURRENT CMAKE BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# Set this path if the conan install is not in the standard directory
# This doesn't work due to a conan internal error when conan is not found in the standard directory (/usr/bin on linux)
# It doesn't work in general when CONAN_COMMAND is set it seems.
#execute_process(COMMAND which conan OUTPUT_VARIABLE CONAN_PATH)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

target_compile_features(project_options
        INTERFACE
        ${PROJ_CXX_STD_FEATURE}
        )
if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" AND ENABLE_BUILD_WITH_TIME_TRACE)
    add_compile_definitions(project_options INTERFACE -ftime-trace)
endif ()
string(
        APPEND _options
        "$<$<CXX_COMPILER_ID:MSVC>:"
            "/permissive-;"
            "$<$<CONFIG:RELEASE>:/O2>"
        ">"
        "$<$<CXX_COMPILER_ID:Clang>:"
            "$<$<CONFIG:RELEASE>:-O3>"
            "$<$<PLATFORM_ID:Darwin>:-stdlib=libc++>"
        ">"
        "$<$<CXX_COMPILER_ID:GNU>:"
            "$<$<CONFIG:RELEASE>:-O3>"
        ">"
)
target_compile_options(project_options INTERFACE "${_options}")


# Set up some extra Conan dependencies based on our needs
# before loading Conan
set(CONAN_EXTRA_REQUIRES "")
set(CONAN_EXTRA_OPTIONS "")

include(${CMAKE_CONFIG_FOLDER}/settings/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

# set the standard project settings
include(cmake/settings/StandardProjectSettings.cmake)

# enable cache system
include(${CMAKE_CONFIG_FOLDER}/settings/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CONFIG_FOLDER}/settings/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(${CMAKE_CONFIG_FOLDER}/settings/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(${CMAKE_CONFIG_FOLDER}/settings/Doxygen.cmake)
enable_doxygen(ENABLE_BUILD_DOCS)

# allow for static analysis options
include(${CMAKE_CONFIG_FOLDER}/settings/StaticAnalyzers.cmake)

if (ENABLE_PCH)
    target_precompile_headers(project_options
            INTERFACE <vector> <string> <map> <utility> <memory> <array>)
endif ()

if (USE_PYBIND11_FINDPYTHON)
    set(PYBIND11_FINDPYTHON TRUE)
else ()
    set(PYBIND11_FINDPYTHON FALSE)
    find_package(Python3 COMPONENTS Interpreter Development)
endif ()
find_package(pybind11 REQUIRED)

include(${CMAKE_CONFIG_FOLDER}/targets/per.cmake)
include(${CMAKE_CONFIG_FOLDER}/targets/pyper.cmake)

if (SKBUILD)
    message("Building with scikit-build. Disabling Test target.")
    # install locally so that sciki-build can correctly install it
    install(TARGETS ${per_pymodule} LIBRARY DESTINATION .)

else ()
    if (ENABLE_TESTING)
        find_package("GTest")
        enable_testing()
        message(
                "Configuring Tests."
        )
        include(${CMAKE_CONFIG_FOLDER}/targets/tests.cmake)
    endif ()

    # in order to use the latest build of the library for a development package install (pip install . -e),
    # we have to install it in the package folder, where it is used
    install(TARGETS ${per_pymodule} LIBRARY DESTINATION ${PROJECT_PYPER_DIR})
    add_custom_target(Copy_${per_pymodule}_to_destination
            COMMAND ${CMAKE_COMMAND} --install)

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion
    )
    install(TARGETS project_options EXPORT ${PROJECT_NAME_LOWERCASE}Options)
    install(TARGETS project_warnings EXPORT ${PROJECT_NAME_LOWERCASE}Warnings)
    install(TARGETS ${per_lib}_interface ${per_lib}_shared ${per_lib}_static
            EXPORT ${PROJECT_NAME_LOWERCASE}Targets
            RUNTIME DESTINATION bin COMPONENT Runtime
            BUNDLE DESTINATION bin COMPONENT Runtime
            LIBRARY DESTINATION lib COMPONENT Runtime
            ARCHIVE DESTINATION lib COMPONENT Development
            PUBLIC_HEADER DESTINATION include COMPONENT Development
            )

    configure_package_config_file(
            "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME_LOWERCASE}Config.cmake.in"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}Config.cmake"
            INSTALL_DESTINATION lib/cmake/${PROJECT_NAME_LOWERCASE}
    )
    install(EXPORT ${PROJECT_NAME_LOWERCASE}Options DESTINATION lib/cmake/${PROJECT_NAME_LOWERCASE})
    install(EXPORT ${PROJECT_NAME_LOWERCASE}Warnings DESTINATION lib/cmake/${PROJECT_NAME_LOWERCASE})
    install(EXPORT ${PROJECT_NAME_LOWERCASE}Targets DESTINATION lib/cmake/${PROJECT_NAME_LOWERCASE})
    install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}ConfigVersion.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}Config.cmake"
            DESTINATION lib/cmake/${PROJECT_NAME_LOWERCASE})
    # this installation assumes that the project has an eponymous include directory for the project c++ library
    # we install that sub-directory instead of PROJECT_PER_INCLUDE_DIR, in order to avoid an
    # include/include/proj_name_inc_dir situation and get the correct include/proj_name_inc_dir.
    install(DIRECTORY ${PROJECT_PER_INCLUDE_DIR}/${PROJECT_NAME_LOWERCASE} DESTINATION include)

endif ()
