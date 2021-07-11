cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCES
        ${PER_SRC_DIR}/sum_tree.cpp
        ${PER_SRC_DIR}/per.cpp
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

target_sources(percpp PRIVATE ${LIBRARY_SOURCES})

target_include_directories(percpp
        PRIVATE
        ${PER_INCLUDE_DIR}
        ${CONAN_INCLUDE_DIRS_PYBIND11}
        ${PYTHON_INCLUDE_DIRS}
        )

set_target_properties(percpp PROPERTIES
        CXX_STANDARD 17
        )
target_compile_features(percpp PRIVATE cxx_std_17)

target_link_libraries(percpp PUBLIC ${PYTHON_LIBRARIES} project_options)

set(PYTHON_MODULE_SOURCES
        ${PER_INCLUDE_DIR}/../binding/module.cpp
)

pybind11_add_module(pyper ${LIBRARY_SOURCES} ${PER_INCLUDE_DIR} ${PYTHON_MODULE_SOURCES})
target_include_directories(pyper
        PRIVATE
        ${PER_INCLUDE_DIR}
        )
