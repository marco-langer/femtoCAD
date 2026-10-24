add_executable(femtoCAD
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    SceneView.cpp
    SceneView.hpp
    Settings.cpp
    Settings.hpp
)

set_target_properties(femtoCAD PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
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
        femtoCAD-core
        femtoCAD::tl-expected
        femtoCAD::ms-gsl
        femtoCAD::qt-widgets
        femtoCAD::warnings
)
