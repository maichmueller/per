set(PYTHON_MODULE_SOURCES
        module.cpp
        init_sumtree.cpp
        init_per.cpp
        )
list(TRANSFORM PYTHON_MODULE_SOURCES PREPEND "${PROJECT_PER_BINDING_DIR}/")

pybind11_add_module(${per_python_module_name} ${LIBRARY_SOURCES} ${PYTHON_MODULE_SOURCES})

set_target_properties(${per_python_module_name} PROPERTIES
        LIBRARY_OUTPUT_NAME _pyper
        CXX_VISIBILITY_PRESET hidden
        )
target_link_libraries(${per_python_module_name} PUBLIC ${per_lib_name}_shared)
