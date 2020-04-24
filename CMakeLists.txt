cmake_minimum_required(VERSION 3.10)

# set project name
project(bw CXX)

# compiler options
add_compile_options(-std=c++17 -g -Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wold-style-cast)
# enables function names in backtraces
set(CMAKE_EXE_LINKER_FLAGS -rdynamic)

# run clang-tidy during compilation
include(cmake/static_analyzers.cmake)

# set include dirs
include_directories(include)
include_directories(src)

# source files
file(GLOB SOURCES_ROOT "src/*.cpp")
file(GLOB SOURCES_SAMPLING "src/sampling/*.cpp")
file(GLOB SOURCES_TERMUI "src/termui/*.cpp")
file(GLOB SOURCES_TOOLS "src/tools/*.cpp")

# targets
add_executable(bw
    ${SOURCES_SAMPLING} ${SOURCES_TERMUI} ${SOURCES_TOOLS} ${SOURCES_ROOT})
