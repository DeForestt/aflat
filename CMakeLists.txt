cmake_minimum_required(VERSION 3.12)
add_subdirectory(Lib/Catch2)

set (CMAKE_CXX_STANDARD 17)
set (GCC_COMPILER_FLAGS -ggdb -g -fpermissive)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
project(aflat VERSION 1.1.0)

file(GLOB_RECURSE all_SRC
    src/*.cpp
)
file(GLOB_RECURSE lsp_SRC
    src/lsp/*.cpp
    src/Scanner.cpp
    src/Exceptions.cpp
)
list(REMOVE_ITEM all_SRC ${lsp_SRC})
set(aflat_SRC ${all_SRC})

# add boost
find_package(Boost 1.71.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(${GCC_COMPILER_FLAGS})
add_executable(aflat ${aflat_SRC})
add_executable(aflat-lsp ${lsp_SRC})
target_compile_definitions(aflat-lsp PRIVATE LSP_KEYWORDS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/lsp/keywords.json")

target_include_directories(aflat PRIVATE include/)
target_include_directories(aflat-lsp PRIVATE include/)

# Testing -->
file(GLOB_RECURSE testing_Modules
    test/*.cpp
    src/CodeGenerator/*.cpp
    src/*.cpp
)

list(REMOVE_ITEM testing_Modules ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM testing_Modules ${CMAKE_CURRENT_SOURCE_DIR}/src/lsp/main.cpp)

add_executable(test ${testing_Modules})
target_include_directories(test PRIVATE include/)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
