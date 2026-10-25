add_library(femtoCAD-core INTERFACE
    include/core/Angle.hpp
    include/core/BoundedValue.hpp
    include/core/Error.hpp
    include/core/NonNegative.hpp
    include/core/Overload.hpp
    include/core/Positive.hpp
    include/core/Result.hpp
    include/core/StrongUuid.hpp
    include/core/VectorExtensions.hpp
)

target_link_libraries(femtoCAD-core
    INTERFACE
        femtoCAD::cxx_standard
        femtoCAD::ms-gsl
        femtoCAD::qt-core
)

target_include_directories(femtoCAD-core INTERFACE include)

add_subdirectory(tests)
