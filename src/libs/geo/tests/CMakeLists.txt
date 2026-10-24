add_executable(femtoCAD-geo-tests
    ArcTests.cpp
    CircleTests.cpp
    InfiniteLineTests.cpp
    LineTests.cpp
)

target_link_libraries(femtoCAD-geo-tests
    PRIVATE
        femtoCAD-geo
        femtoCAD-test_utils
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-geo-tests)
