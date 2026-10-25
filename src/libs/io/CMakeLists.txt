add_library(femtoCAD-io STATIC
    include/io/DxfImport.hpp
    include/io/DxfImportError.hpp
    src/DxfColors.cpp
    src/DxfColors.hpp
    src/DxfEntities.cpp
    src/DxfEntities.hpp
    src/DxfImport.cpp
    src/DxfImportError.cpp
    src/DxfLineweights.cpp
    src/DxfLineweights.hpp
    src/DxfUnits.cpp
    src/DxfUnits.hpp
)

target_link_libraries(femtoCAD-io
    PUBLIC
        femtoCAD-domain
        femtoCAD::qt-core
    PRIVATE
        femtoCAD::ms-gsl
        femtoCAD::opendxf
        femtoCAD::warnings
)

target_include_directories(femtoCAD-io PUBLIC include)

add_subdirectory(tests)
