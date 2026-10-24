add_executable(femtoCAD-geo-tests
    ArcTests.cpp
    CircleTests.cpp
    InfiniteLineTests.cpp
    LineTests.cpp
)

set_target_properties(femtoCAD-geo-tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(femtoCAD-geo-tests
    PRIVATE
        femtoCAD-geo
        femtoCAD-test_utils
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-geo-tests)
