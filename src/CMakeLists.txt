cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCES
        ${PER_SRC_DIR}/sum_tree.cpp
        ${PER_SRC_DIR}/per.cpp
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

target_sources(per_cpp_lib PRIVATE ${LIBRARY_SOURCES})

target_include_directories(per_cpp_lib
        PRIVATE
        ${PER_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_PYBIND11}
        ${Python_INCLUDE_DIRS}
        )

set_target_properties(per_cpp_lib PROPERTIES
        CXX_STANDARD 17
        CXX_VISIBILITY_PRESET hidden
        )

target_compile_features(per_cpp_lib PRIVATE cxx_std_17)

target_link_libraries(per_cpp_lib PUBLIC ${PYTHON_LIBRARIES} project_options)

set(PYTHON_MODULE_SOURCES
        ${PER_INCLUDE_DIR}/../binding/module.cpp
        ${PER_INCLUDE_DIR}/../binding/init_sumtree.cpp
        ${PER_INCLUDE_DIR}/../binding/init_per.cpp
)

pybind11_add_module(_pyper ${LIBRARY_SOURCES} ${PYTHON_MODULE_SOURCES})

set_target_properties(_pyper PROPERTIES
        CXX_STANDARD 17
        CXX_VISIBILITY_PRESET hidden
        )
target_include_directories(_pyper
        PRIVATE
        ${PER_INCLUDE_DIR}
        )
