add_library(femtoCAD-core INTERFACE
    include/core/primitives/NonNegative.hpp
    include/core/units/Angle.hpp
    include/core/utility/VectorExtensions.hpp
)

target_link_libraries(femtoCAD-core
    INTERFACE
        femtoCAD::fmt
        femtoCAD::ms-gsl
)

target_include_directories(femtoCAD-core INTERFACE include)
