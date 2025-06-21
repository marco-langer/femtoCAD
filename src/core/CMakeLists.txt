add_library(femtoCAD-core STATIC
    include/core/BoundingBox.hpp
    include/core/Circle.hpp
    include/core/Coordinate.hpp
    include/core/Layer.hpp
    include/core/Line.hpp
    include/core/Workspace.hpp
    BoundingBox.cpp
    Circle.cpp
    Coordinate.cpp
    Layer.cpp
    Line.cpp
    Workspace.cpp
)

target_include_directories(femtoCAD-core PUBLIC include)

set_target_properties(femtoCAD-core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
