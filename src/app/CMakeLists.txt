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

target_link_libraries(femtoCAD
    PRIVATE
        femtoCAD-core
        femtoCAD::tl-expected
        femtoCAD::qt-widgets
        femtoCAD::warnings
)
