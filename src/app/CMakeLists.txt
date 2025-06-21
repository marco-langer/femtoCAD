find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

message(STATUS "Using Qt v.${Qt6_VERSION}")

set(CMAKE_AUTOMOC ON)

add_executable(femtoCAD
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    SceneView.cpp
    SceneView.hpp
)

set_target_properties(femtoCAD PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(femtoCAD
    PRIVATE
        femtoCAD-core
        Qt6::Widgets
        Qt6::OpenGLWidgets
)
