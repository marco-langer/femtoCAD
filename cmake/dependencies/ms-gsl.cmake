find_package(Microsoft.GSL CONFIG REQUIRED)

message(STATUS "Using Microsoft.GSL v.${Microsoft.GSL_VERSION}")

add_library(_femtoCAD_ms-gsl INTERFACE)
add_library(femtoCAD::ms-gsl ALIAS _femtoCAD_ms-gsl)

target_link_libraries(_femtoCAD_ms-gsl
    INTERFACE
        Microsoft.GSL::GSL
)
