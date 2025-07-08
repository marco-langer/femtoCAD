add_library(_femtoCAD_warnings INTERFACE)
add_library(femtoCAD::warnings ALIAS _femtoCAD_warnings)

target_compile_options(_femtoCAD_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wcast-align
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wmissing-declarations
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        -Wzero-as-null-pointer-constant
        -Wsuggest-override
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wundef
        -Werror
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
    >
)
