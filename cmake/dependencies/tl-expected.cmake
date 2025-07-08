find_package(tl-expected CONFIG REQUIRED)

message(STATUS "Using tl-expected v.${tl-expected_VERSION}")

add_library(_femtoCAD_tl-expected INTERFACE)
add_library(femtoCAD::tl-expected ALIAS _femtoCAD_tl-expected)

target_link_libraries(_femtoCAD_tl-expected
    INTERFACE
        tl::expected
)
