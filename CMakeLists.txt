cmake_minimum_required(VERSION 3.12)
add_subdirectory(Lib/Catch2)

set (CMAKE_CXX_STANDARD 17)
set (GCC_COMPILER_FLAGS -O3 -march=native -fpermissive)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
project(aflat VERSION 1.1.0)

file(GLOB_RECURSE aflat_SRC CONFIGURE_DEPENDS
    src/*.cpp
) 

# add boost
find_package(Boost 1.71.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_compile_options(${GCC_COMPILER_FLAGS})
# threading
find_package(Threads REQUIRED)
add_executable(aflat ${aflat_SRC})
target_link_libraries(aflat PRIVATE Threads::Threads)
# target_link_options(aflat PRIVATE -flto)

target_include_directories(aflat PRIVATE include/) 

# Testing -->
file(GLOB_RECURSE testing_Modules CONFIGURE_DEPENDS
    test/*.cpp
    src/CodeGenerator/*.cpp
    src/*.cpp
)

# Do not remove main.cpp when building tests so helper functions are available
# The main function is excluded via the AFLAT_TEST define.

message (STATUS ${testing_Modules})
add_executable(a.test ${testing_Modules})
target_include_directories(a.test PRIVATE include/)
target_link_libraries(a.test PRIVATE Catch2::Catch2WithMain Threads::Threads)
target_compile_definitions(a.test PRIVATE AFLAT_TEST)
# target_link_options(test PRIVATE -flto)
