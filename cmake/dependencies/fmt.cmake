find_package(fmt CONFIG REQUIRED)

message(STATUS "Using fmt v.${fmt_VERSION}")

add_library(_femtoCAD_fmt INTERFACE)
add_library(femtoCAD::fmt ALIAS _femtoCAD_fmt)

target_link_libraries(_femtoCAD_fmt
    INTERFACE
        fmt::fmt
)
