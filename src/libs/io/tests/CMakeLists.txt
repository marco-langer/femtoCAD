add_executable(femtoCAD-io-tests
    DxfImportTests.cpp
    matchers/LayerMatcher.cpp
    matchers/LayerMatcher.hpp
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
