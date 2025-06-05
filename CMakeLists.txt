cmake_minimum_required(VERSION 3.12)
add_subdirectory(Lib/Catch2)

set (CMAKE_CXX_STANDARD 17)
set (GCC_COMPILER_FLAGS -O3 -march=native -flto -fpermissive)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
project(aflat VERSION 1.1.0)

file(GLOB_RECURSE aflat_SRC
    src/*.cpp
) 

# add boost
find_package(Boost 1.71.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_compile_options(${GCC_COMPILER_FLAGS})
add_executable(aflat ${aflat_SRC})
target_link_options(aflat PRIVATE -flto)

target_include_directories(aflat PRIVATE include/) 

# Testing -->
file(GLOB_RECURSE testing_Modules
    test/*.cpp
    src/CodeGenerator/*.cpp
    src/*.cpp
)

list(REMOVE_ITEM testing_Modules ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

message (STATUS ${testing_Modules})
add_executable(test ${testing_Modules})
target_include_directories(test PRIVATE include/)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_link_options(test PRIVATE -flto)
