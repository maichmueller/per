
find_package("GTest")

set(TEST_SOURCES
        test_sumtree.cpp
        tests.cpp)

add_executable(tests ${TEST_SOURCES})

set_target_properties(tests PROPERTIES
        CXX_STANDARD 17
        CXX_VISIBILITY_PRESET hidden
        )

target_include_directories(tests PRIVATE
        ${PER_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_GTEST}
        ${CONAN_INCLUDE_DIRS_PYBIND11}
        ${Python_INCLUDE_DIRS})

target_link_libraries(tests PRIVATE
        per_cpp_lib
        CONAN_PKG::gtest
        pybind11::embed
        ${Python_LIBRARIES})

