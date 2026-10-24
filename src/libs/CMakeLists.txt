find_package(GTest CONFIG REQUIRED)
message(STATUS "Using GTest v.${GTest_VERSION}")

include(GoogleTest)

add_subdirectory(test_utils)
add_subdirectory(domain)
add_subdirectory(io)
