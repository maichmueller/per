cmake_minimum_required(VERSION 3.16)

project(PER_INSTALL LANGUAGES CXX)

find_package(per REQUIRED
        PATHS ${per_path}
        )

add_executable(per_install_test main.cpp)
target_link_libraries(per_install_test PRIVATE per++_static)