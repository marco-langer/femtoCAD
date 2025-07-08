find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

message(STATUS "Using Qt v.${Qt6_VERSION}")

set(CMAKE_AUTOMOC ON)

add_library(_femtoCAD_qt-widgets INTERFACE)
add_library(femtoCAD::qt-widgets ALIAS _femtoCAD_qt-widgets)

target_link_libraries(_femtoCAD_qt-widgets
    INTERFACE
        Qt6::Widgets
        Qt6::OpenGLWidgets
)

add_library(_femtoCAD_qt-core INTERFACE)
add_library(femtoCAD::qt-core ALIAS _femtoCAD_qt-core)

target_link_libraries(_femtoCAD_qt-core
    INTERFACE
        Qt6::Core
)
