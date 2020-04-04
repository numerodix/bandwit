option(ENABLE_CLANG_TIDY "Enable clang-tidy during compilation" OFF)

if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy;
        -header-filter=.;
        -checks=*,-cert-env33-c,-fuchsia-*,-llvm-header-guard,-misc-macro-parentheses;)
endif()
