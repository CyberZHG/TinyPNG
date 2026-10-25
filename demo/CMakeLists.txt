cmake_minimum_required(VERSION 4.0)
project(demo)

set(CMAKE_CXX_STANDARD 20)

option(TINY_PNG_DEMO_USE_LOCAL "Use the TinyPNG source in the parent directory" OFF)

add_executable(demo
        main.cpp
)

if(TINY_PNG_DEMO_USE_LOCAL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/TinyPNG)
else()
    include(FetchContent)

    FetchContent_Declare(
            TinyPNG
            GIT_REPOSITORY https://github.com/CyberZHG/TinyPNG.git
            # GIT_REPOSITORY git@github.com:CyberZHG/TinyPNG.git
            GIT_TAG main
    )

    FetchContent_MakeAvailable(TinyPNG)
endif()

target_link_libraries(demo PRIVATE TinyPNG)
