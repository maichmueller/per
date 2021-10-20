set(PYTHON_MODULE_SOURCES
        module.cpp
        init_sumtree.cpp
        init_per.cpp
        )
list(TRANSFORM PYTHON_MODULE_SOURCES PREPEND "${PROJECT_PER_BINDING_DIR}/")

pybind11_add_module(${per_pymodule} ${LIBRARY_SOURCES} ${PYTHON_MODULE_SOURCES})

set_target_properties(${per_pymodule} PROPERTIES
        LIBRARY_OUTPUT_NAME _pyper
        CXX_VISIBILITY_PRESET hidden
        )
target_link_libraries(${per_pymodule} PUBLIC ${per_lib}_shared)
