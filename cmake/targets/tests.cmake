
set(TEST_SOURCES
        test_sumtree.cpp
        test_per.cpp
        tests.cpp
        )
list(TRANSFORM TEST_SOURCES PREPEND "${PROJECT_TEST_DIR}/")

add_executable(${per_test_bin_name} ${TEST_SOURCES})

set_target_properties(${per_test_bin_name} PROPERTIES
        EXCLUDE_FROM_ALL True  # don't build tests when ALL is asked to be built. Only on demand.
        )

target_link_libraries(${per_test_bin_name}
        PRIVATE
        ${per_lib_name}_static
        CONAN_PKG::gtest
        )

add_test(
        NAME Test_${PROJECT_NAME}
        COMMAND ${per_test_bin_name}
)