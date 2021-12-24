message("Configuring Docs.")
set(DOXYGEN_CALLER_GRAPH YES)
set(DOXYGEN_CALL_GRAPH YES)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_USE_MATHJAX YES)
find_package(Doxygen REQUIRED dot)
doxygen_add_docs(
        doxygen-docs
        .
        EXCLUDE_FROM_ALL
        WORKING_DIRECTORY ${_docs_DOXYGEN_WORKING_DIR}
        COMMENT "Generating docs with doxygen."
)
