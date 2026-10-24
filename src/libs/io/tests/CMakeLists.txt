add_executable(femtoCAD-io-tests
    DxfImportTests.cpp
    matchers/LayerMatcher.cpp
    matchers/LayerMatcher.hpp
)

set_target_properties(femtoCAD-io-tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_definitions(femtoCAD-io-tests
    PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/TestData/"
)

target_link_libraries(femtoCAD-io-tests
    PRIVATE
        femtoCAD-io
        femtoCAD-test_utils
        femtoCAD::qt-core
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-io-tests)
