find_package(GTest CONFIG REQUIRED)
message(STATUS "Using GTest v.${GTest_VERSION}")

include(GoogleTest)

add_executable(femtoCAD-core-tests
    Matchers/LayerMatcher.cpp
    Matchers/LayerMatcher.hpp
    AngleTests.cpp
    ArcTests.cpp
    CircleTests.cpp
    DxfImportTests.cpp
    InfiniteLineTests.cpp
    LayerTests.cpp
    LineTests.cpp
    NonNegativeTests.cpp
)

set_target_properties(femtoCAD-core-tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_definitions(femtoCAD-core-tests
    PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/TestData/"
)

target_link_libraries(femtoCAD-core-tests
    PRIVATE
        femtoCAD-core
        femtoCAD-test_utils
        femtoCAD::qt-core
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-core-tests)
