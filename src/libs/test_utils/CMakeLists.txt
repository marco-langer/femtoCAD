add_library(femtoCAD-test_utils STATIC
    include/test_utils/matchers/ArcMatcher.hpp
    include/test_utils/matchers/BoundingBoxMatcher.hpp
    include/test_utils/matchers/CircleMatcher.hpp
    include/test_utils/matchers/CoordinateMatcher.hpp
    include/test_utils/matchers/LineMatcher.hpp
    include/test_utils/matchers/RadiusMatcher.hpp
    src/matchers/ArcMatcher.cpp
    src/matchers/BoundingBoxMatcher.cpp
    src/matchers/CircleMatcher.cpp
    src/matchers/CoordinateMatcher.cpp
    src/matchers/LineMatcher.cpp
    src/matchers/RadiusMatcher.cpp
)

target_link_libraries(femtoCAD-test_utils
    PUBLIC
        femtoCAD-geo
        GTest::gmock
    PRIVATE
        femtoCAD::warnings
)

target_include_directories(femtoCAD-test_utils PUBLIC include)
