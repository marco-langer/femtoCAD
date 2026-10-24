cmake_minimum_required(VERSION 3.25.1)

project(femtoCAD LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)
include(cxx_standard)
include(warnings)

# Set after include(Dependencies) so it doesn't affect the FetchContent dependencies.
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(src)
