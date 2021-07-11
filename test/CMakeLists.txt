
find_package("GTest")

set(TEST_SOURCES
        test_sumtree.cpp
        tests.cpp)

add_executable(tests ${TEST_SOURCES})

target_include_directories(tests PRIVATE
        ${PER_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_GTEST}
        ${CONAN_INCLUDE_DIRS_PYBIND11}
        ${PYTHON_INCLUDE_DIRS})

target_link_libraries(tests PRIVATE
        project_options
        percpp
        CONAN_PKG::gtest
        pybind11::embed
        ${PYTHON_LIBRARIES})

