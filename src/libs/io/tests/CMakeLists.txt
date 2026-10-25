add_executable(femtoCAD-io-tests
    DxfColorsTests.cpp
    DxfEntitiesTests.cpp
    DxfImportTests.cpp
    DxfUnitsTests.cpp
    matchers/LayerMatcher.cpp
    matchers/LayerMatcher.hpp
)

target_compile_definitions(femtoCAD-io-tests
    PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/TestData/"
)

# The tests also cover the private headers in src/.
target_include_directories(femtoCAD-io-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(femtoCAD-io-tests
    PRIVATE
        femtoCAD-io
        femtoCAD::opendxf
        femtoCAD-test_utils
        femtoCAD::qt-core
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-io-tests)
