cmake_minimum_required(VERSION 3.16)

project(PER_INSTALL LANGUAGES CXX)

set(DEPENDENCY_DIR ../../deps)  # has to be relative to CMAKE_CURRENT_SOURCE_DIR
set(CONANFILE conanfile.txt)

set(_cmake_DIR ../../cmake/)
include(${_cmake_DIR}/settings/Conan.cmake)
include(${_cmake_DIR}/settings/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(pybind11 REQUIRED)
find_package(per REQUIRED CONFIG
        PATHS ${per_path}
        )

add_executable(per_install_test_static main.cpp)
target_link_libraries(per_install_test_static PRIVATE per::per++_static)
add_executable(per_install_test_shared main.cpp)
target_link_libraries(per_install_test_shared PRIVATE per::per++_shared)