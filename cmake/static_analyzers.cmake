option(ENABLE_CLANG_TIDY "Enable clang-tidy during compilation" OFF)

if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()
