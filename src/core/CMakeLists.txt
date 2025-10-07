add_library(femtoCAD-core STATIC
    include/core/io/DxfImport.hpp
    include/core/primitives/NonNegative.hpp
    include/core/primitives/Radius.hpp
    include/core/units/Angle.hpp
    include/core/utility/VectorExtensions.hpp
    include/core/Arc.hpp
    include/core/BoundingBox.hpp
    include/core/Circle.hpp
    include/core/Coordinate.hpp
    include/core/InfiniteLine.hpp
    include/core/Layer.hpp
    include/core/Line.hpp
    include/core/Polyline.hpp
    include/core/Workspace.hpp
    io/DxfImport.cpp
    Arc.cpp
    BoundingBox.cpp
    Circle.cpp
    Coordinate.cpp
    InfiniteLine.cpp
    Layer.cpp
    Line.cpp
    Polyline.cpp
    Workspace.cpp
)

target_link_libraries(femtoCAD-core
    PRIVATE
        femtoCAD::opendxf
        femtoCAD::tl-expected
        femtoCAD::qt-core
        femtoCAD::warnings
)

target_include_directories(femtoCAD-core PUBLIC include)

set_target_properties(femtoCAD-core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
