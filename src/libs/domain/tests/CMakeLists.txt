add_executable(femtoCAD-domain-tests
    ColorTests.cpp
    EntityTests.cpp
    LayerTests.cpp
    LinetypeTests.cpp
    LineweightTests.cpp
)

target_link_libraries(femtoCAD-domain-tests
    PRIVATE
        femtoCAD-domain
        femtoCAD-test_utils
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-domain-tests)
