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
    test/*.cpp
    src/CodeGenerator/*.cpp
    src/*.cpp
)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS *boost libraries here*) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(progname file1.cxx file2.cxx) 
    target_link_libraries(progname ${Boost_LIBRARIES})
endif()

list(REMOVE_ITEM testing_Modules ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

message (STATUS ${testing_Modules})
add_executable(test ${testing_Modules})
target_include_directories(test PRIVATE include/)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)