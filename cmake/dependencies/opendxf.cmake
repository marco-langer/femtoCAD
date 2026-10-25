include(FetchContent)

FetchContent_Declare(
    opendxf
    GIT_REPOSITORY https://github.com/marco-langer/opendxf.git
    GIT_TAG        3f167fc6bd524bd0e5a4c00fdb695bd8487b2521
)

FetchContent_MakeAvailable(opendxf)

add_library(_femtoCAD_opendxf INTERFACE)
add_library(femtoCAD::opendxf ALIAS _femtoCAD_opendxf)

target_link_libraries(_femtoCAD_opendxf
    INTERFACE
        opendxf
)
