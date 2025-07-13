include(FetchContent)

FetchContent_Declare(
    opendxf
    GIT_REPOSITORY https://github.com/marco-langer/opendxf.git
    GIT_TAG        09962ffa5bdf093aff0bc7ee1d422a6397d4b1f5
)

FetchContent_MakeAvailable(opendxf)

add_library(_femtoCAD_opendxf INTERFACE)
add_library(femtoCAD::opendxf ALIAS _femtoCAD_opendxf)

target_link_libraries(_femtoCAD_opendxf
    INTERFACE
        opendxf
)
