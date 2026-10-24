add_library(femtoCAD-geo STATIC
    include/geo/Arc.hpp
    include/geo/BoundingBox.hpp
    include/geo/Circle.hpp
    include/geo/Coordinate.hpp
    include/geo/InfiniteLine.hpp
    include/geo/Line.hpp
    include/geo/Radius.hpp
    src/Arc.cpp
    src/BoundingBox.cpp
    src/Circle.cpp
    src/Coordinate.cpp
    src/InfiniteLine.cpp
    src/Line.cpp
)

target_link_libraries(femtoCAD-geo
    PUBLIC
        femtoCAD-core
    PRIVATE
        femtoCAD::warnings
)

target_include_directories(femtoCAD-geo PUBLIC include)

set_target_properties(femtoCAD-geo PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_subdirectory(tests)
