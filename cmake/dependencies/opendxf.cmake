include(FetchContent)

FetchContent_Declare(
    opendxf
    GIT_REPOSITORY https://github.com/marco-langer/opendxf.git
    GIT_TAG        a76047c315f3508fe6eb2b58a2e414a32e1b41d7
)

FetchContent_MakeAvailable(opendxf)

add_library(_femtoCAD_opendxf INTERFACE)
add_library(femtoCAD::opendxf ALIAS _femtoCAD_opendxf)

target_link_libraries(_femtoCAD_opendxf
    INTERFACE
        opendxf
)
