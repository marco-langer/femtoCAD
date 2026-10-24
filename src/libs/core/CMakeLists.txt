add_library(femtoCAD-core INTERFACE
    include/core/Angle.hpp
    include/core/NonNegative.hpp
    include/core/VectorExtensions.hpp
)

target_link_libraries(femtoCAD-core
    INTERFACE
        femtoCAD::fmt
        femtoCAD::ms-gsl
)

target_include_directories(femtoCAD-core INTERFACE include)

add_subdirectory(tests)
