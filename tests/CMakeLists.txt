find_package(GTest CONFIG REQUIRED)
message(STATUS "Using GTest v.${GTest_VERSION}")

include(GoogleTest)

add_executable(femtoCAD-core-tests
    Matchers/BoundingBoxMatcher.cpp
    Matchers/BoundingBoxMatcher.hpp
    Matchers/CoordinateMatcher.cpp
    Matchers/CoordinateMatcher.hpp
    Matchers/LayerMatcher.cpp
    Matchers/LayerMatcher.hpp
    Matchers/LineMatcher.cpp
    Matchers/LineMatcher.hpp
    CircleTests.cpp
    DxfImportTests.cpp
    LayerTests.cpp
    LineTests.cpp
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
        femtoCAD::qt-core
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-core-tests)
