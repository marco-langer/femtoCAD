add_library(femtoCAD-domain STATIC
    include/domain/Document.hpp
    include/domain/Layer.hpp
    include/domain/Workspace.hpp
    src/Document.cpp
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

add_subdirectory(tests)
