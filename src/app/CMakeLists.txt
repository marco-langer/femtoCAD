add_executable(femtoCAD
    features/io/dxf/LengthUnitDialog.cpp
    features/io/dxf/LengthUnitDialog.hpp
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    SceneView.cpp
    SceneView.hpp
    Settings.cpp
    Settings.hpp
)

# Includes are relative to src/app, e.g. "features/io/dxf/LengthUnitDialog.hpp".
target_include_directories(femtoCAD PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_resources(femtoCAD shaders
    PREFIX /shaders
    BASE ${PROJECT_SOURCE_DIR}/shaders
    FILES
        ${PROJECT_SOURCE_DIR}/shaders/background.frag
        ${PROJECT_SOURCE_DIR}/shaders/background.vert
        ${PROJECT_SOURCE_DIR}/shaders/line.frag
        ${PROJECT_SOURCE_DIR}/shaders/line.vert
)

target_link_libraries(femtoCAD
    PRIVATE
        femtoCAD-core
        femtoCAD-domain
        femtoCAD-io
        femtoCAD::ms-gsl
        femtoCAD::qt-widgets
        femtoCAD::warnings
)
