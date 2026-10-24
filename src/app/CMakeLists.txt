add_executable(femtoCAD
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    SceneView.cpp
    SceneView.hpp
    Settings.cpp
    Settings.hpp
)

qt_add_resources(femtoCAD shaders
    PREFIX /shaders
    BASE ${PROJECT_SOURCE_DIR}/shaders
    FILES
        ${PROJECT_SOURCE_DIR}/shaders/line.frag
        ${PROJECT_SOURCE_DIR}/shaders/line.vert
)

target_link_libraries(femtoCAD
    PRIVATE
        femtoCAD-domain
        femtoCAD-io
        femtoCAD::tl-expected
        femtoCAD::ms-gsl
        femtoCAD::qt-widgets
        femtoCAD::warnings
)
