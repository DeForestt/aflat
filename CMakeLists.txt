cmake_minimum_required(VERSION 3.12)
add_subdirectory(Lib/Catch2)

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

# Testing -->
file(GLOB_RECURSE testing_Modules
    test/test.cpp
    src/CodeGenerator/*.cpp
    src/*.cpp
)

list(REMOVE_ITEM testing_Modules /home/deforestt/repos/aflat/src/main.cpp)

message (STATUS ${testing_Modules})
add_executable(test ${testing_Modules})
target_include_directories(test PRIVATE include/)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)