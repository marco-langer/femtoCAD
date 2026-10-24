add_library(femtoCAD-io STATIC
    include/io/DxfImport.hpp
    include/io/DxfImportError.hpp
    src/DxfImport.cpp
    src/DxfImportError.cpp
)

target_link_libraries(femtoCAD-io
    PUBLIC
        femtoCAD-domain
        femtoCAD::qt-core
    PRIVATE
        femtoCAD::opendxf
        femtoCAD::warnings
)

target_include_directories(femtoCAD-io PUBLIC include)

add_subdirectory(tests)
