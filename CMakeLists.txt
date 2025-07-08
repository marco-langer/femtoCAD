cmake_minimum_required(VERSION 3.25.1)

project(femtoCAD LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)
include(warnings)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
