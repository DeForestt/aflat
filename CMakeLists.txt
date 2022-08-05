cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 17)
set (GCC_COMPILER_FLAGS -ggdb -g -fpermissive)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
project(aflat VERSION 1.1.0)

file(GLOB_RECURSE aflat_SRC
    src/*.cpp
)

add_definitions(${GCC_COMPILER_FLAGS})
add_executable(aflat ${aflat_SRC})

target_include_directories(aflat PRIVATE include/)
