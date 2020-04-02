cmake_minimum_required(VERSION 3.5)

# set project name
project(bmon CXX)

# compiler options
add_compile_options(-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wold-style-cast)

# run clang-tidy during compilation
set(CMAKE_CXX_CLANG_TIDY clang-tidy;
    -header-filter=.;
    -checks=*,-fuchsia-*,-llvm-header-guard;)

# set include dirs
include_directories(include)
include_directories(src)

# source files
file(GLOB SOURCES "src/*.cpp")

# targets
add_executable(bmon ${SOURCES})
set_target_properties(bmon PROPERTIES CXX_STANDARD 17)