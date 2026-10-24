add_library(femtoCAD-io STATIC
    include/io/DxfImport.hpp
    src/DxfImport.cpp
)

target_link_libraries(femtoCAD-io
    PUBLIC
        femtoCAD-core
        femtoCAD::qt-core
        femtoCAD::tl-expected
    PRIVATE
        femtoCAD::opendxf
        femtoCAD::warnings
)

target_include_directories(femtoCAD-io PUBLIC include)

set_target_properties(femtoCAD-io PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_subdirectory(tests)
