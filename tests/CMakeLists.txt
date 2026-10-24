find_package(GTest CONFIG REQUIRED)
message(STATUS "Using GTest v.${GTest_VERSION}")

include(GoogleTest)

add_executable(femtoCAD-core-tests
    AngleTests.cpp
    ArcTests.cpp
    CircleTests.cpp
    InfiniteLineTests.cpp
    LineTests.cpp
    NonNegativeTests.cpp
)

set_target_properties(femtoCAD-core-tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(femtoCAD-core-tests
    PRIVATE
        femtoCAD-core
        femtoCAD-test_utils
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-core-tests)
