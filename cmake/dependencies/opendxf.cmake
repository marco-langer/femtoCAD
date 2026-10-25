include(FetchContent)

FetchContent_Declare(
    opendxf
    GIT_REPOSITORY https://github.com/marco-langer/opendxf.git
    GIT_TAG        7da0a9d8b9063811b625f3d266ef74a0914fa68c
)

FetchContent_MakeAvailable(opendxf)

add_library(_femtoCAD_opendxf INTERFACE)
add_library(femtoCAD::opendxf ALIAS _femtoCAD_opendxf)

target_link_libraries(_femtoCAD_opendxf
    INTERFACE
        opendxf
)
