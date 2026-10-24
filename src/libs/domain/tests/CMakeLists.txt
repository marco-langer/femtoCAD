add_executable(femtoCAD-domain-tests
    LayerTests.cpp
)

set_target_properties(femtoCAD-domain-tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(femtoCAD-domain-tests
    PRIVATE
        femtoCAD-domain
        femtoCAD-test_utils
        femtoCAD::warnings
        GTest::gmock_main
)

gtest_discover_tests(femtoCAD-domain-tests)
