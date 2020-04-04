cmake_minimum_required(VERSION 3.10)

# set project name
project(bmon CXX)

# compiler options
add_compile_options(-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wold-style-cast)

# run clang-tidy during compilation
set(CMAKE_CXX_CLANG_TIDY clang-tidy;
    -header-filter=.;
    -checks=*,-cert-env33-c,-fuchsia-*,-llvm-header-guard,-misc-macro-parentheses;)

# set include dirs
include_directories(include)
include_directories(src)

# source files
file(GLOB SOURCES_ROOT "src/*.cpp")
file(GLOB SOURCES_SAMPLING "src/sampling/*.cpp")

# targets
add_executable(bmon ${SOURCES_ROOT} ${SOURCES_SAMPLING})
set_target_properties(bmon PROPERTIES CXX_STANDARD 17)