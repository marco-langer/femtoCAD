add_executable(femtoCAD-core-tests
    AngleTests.cpp
    NonNegativeTests.cpp
    OverloadTests.cpp
    StrongUuidTests.cpp
)

target_link_libraries(femtoCAD-core-tests
    PRIVATE
        femtoCAD-core
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-core-tests)
