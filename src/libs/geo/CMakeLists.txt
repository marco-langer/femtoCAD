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

add_subdirectory(tests)
