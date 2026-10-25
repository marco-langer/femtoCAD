add_library(femtoCAD-domain STATIC
    include/domain/ByLayer.hpp
    include/domain/Color.hpp
    include/domain/Document.hpp
    include/domain/Entity.hpp
    include/domain/Layer.hpp
    include/domain/LengthUnit.hpp
    include/domain/Linetype.hpp
    include/domain/Lineweight.hpp
    include/domain/Workspace.hpp
    src/Color.cpp
    src/Document.cpp
    src/Layer.cpp
    src/Linetype.cpp
    src/Lineweight.cpp
    src/Workspace.cpp
)

target_link_libraries(femtoCAD-domain
    PUBLIC
        femtoCAD-core
        femtoCAD-geo
        femtoCAD::qt-core
    PRIVATE
        femtoCAD::warnings
)

target_include_directories(femtoCAD-domain PUBLIC include)

add_subdirectory(tests)
