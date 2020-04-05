cmake_minimum_required(VERSION 3.10)

# set project name
project(bmon CXX)

# compiler options
add_compile_options(-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wold-style-cast)

# run clang-tidy during compilation
include(cmake/static_analyzers.cmake)

# set include dirs
include_directories(include)
include_directories(src)

# source files
file(GLOB SOURCES_ROOT "src/*.cpp")
file(GLOB SOURCES_SAMPLING "src/sampling/*.cpp")
file(GLOB SOURCES_TERMUI "src/termui/*.cpp")

# targets
add_executable(bmon ${SOURCES_SAMPLING} ${SOURCES_TERMUI} ${SOURCES_ROOT})
set_target_properties(bmon PROPERTIES CXX_STANDARD 17)