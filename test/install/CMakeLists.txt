cmake_minimum_required(VERSION 3.21)
project(PER_INSTALL LANGUAGE CXX)

set(CXX_STANDARD C++17)

find_package(per REQUIRED)

add_executable(per_install_test main.cpp)
target_link_libraries(per_install_test PRIVATE per++_static)