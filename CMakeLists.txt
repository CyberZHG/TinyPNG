cmake_minimum_required(VERSION 4.0)
project(TinyPNG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(TINY_PNG_ENABLE_TESTS "Build tests" OFF)
option(TINY_PNG_ENABLE_STRICT "Use strict compile options" OFF)
option(TINY_PNG_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(TINY_PNG_BIND_PYTHON "Enable Python binding" OFF)
option(TINY_PNG_BIND_ES "Enable ECMAScript binding" OFF)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

if(TINY_PNG_ENABLE_STRICT)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(
                -Wall
                -Wextra
                -Werror
                -Wpedantic
                -Wmissing-include-dirs
                -Wundef
                -Wredundant-decls
        )
    endif()
endif()

if(TINY_PNG_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g -fno-elide-constructors)
        add_link_options(--coverage)
    endif()
endif()

add_library(TinyPNG STATIC
        include/constants.h
        include/lz77.h
        src/lz77.cpp
        include/bit_stream.h
        src/bit_stream.cpp
        include/huffman.h
        src/huffman.cpp
        include/deflate.h
        src/deflate.cpp
)

target_include_directories(TinyPNG
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(TINY_PNG_ENABLE_TESTS)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(runTests
            tests/test_lz77.cpp
            tests/test_bit_stream.cpp
            tests/test_huffman.cpp
            tests/test_deflate.cpp
    )

    target_link_libraries(runTests
            PRIVATE TinyPNG
            PRIVATE gtest gtest_main
    )

    add_test(NAME TinyPNGTests COMMAND runTests)
endif()

if(TINY_PNG_BIND_PYTHON)
    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG v3.0
    )
    FetchContent_MakeAvailable(pybind11)

    pybind11_add_module(_core
            python/binding.cpp
    )
    target_link_libraries(_core
            PRIVATE TinyPNG
    )
    install(TARGETS _core
            LIBRARY DESTINATION tiny_png)
    install(FILES python/wrapper/__init__.py DESTINATION tiny_png)
endif()

if(TINY_PNG_BIND_ES)
    add_executable(TinyPNGWASM
            wasm/binding.cpp
    )

    target_link_libraries(TinyPNGWASM
            PRIVATE TinyPNG
    )

    target_compile_options(TinyPNGWASM PRIVATE "-O3")

    set_target_properties(TinyPNGWASM PROPERTIES SUFFIX ".js")

    target_link_options(TinyPNGWASM PRIVATE
            "--bind"
            "-sMODULARIZE=1"
            "-sEXPORT_ES6=1"
            "-sEXPORT_NAME=SpikyNesWASMModule"
            "-sALLOW_MEMORY_GROWTH=1"
    )
endif()
