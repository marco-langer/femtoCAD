include(FetchContent)

FetchContent_Declare(
    opendxf
    GIT_REPOSITORY https://github.com/marco-langer/opendxf.git
    GIT_TAG        c226add313d57efd89533e0cb78f90630d4ce5f6
)

FetchContent_MakeAvailable(opendxf)

add_library(_femtoCAD_opendxf INTERFACE)
add_library(femtoCAD::opendxf ALIAS _femtoCAD_opendxf)

target_link_libraries(_femtoCAD_opendxf
    INTERFACE
        opendxf
)
