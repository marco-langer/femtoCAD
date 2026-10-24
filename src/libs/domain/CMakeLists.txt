add_library(femtoCAD-domain STATIC
    include/domain/Layer.hpp
    include/domain/Workspace.hpp
    src/Layer.cpp
    src/Workspace.cpp
)

target_link_libraries(femtoCAD-domain
    PUBLIC
        femtoCAD-geo
        femtoCAD::qt-core
    PRIVATE
        femtoCAD::warnings
)

target_include_directories(femtoCAD-domain PUBLIC include)

set_target_properties(femtoCAD-domain PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_subdirectory(tests)
