cmake_minimum_required(VERSION 3.12)
add_subdirectory(Lib/Catch2)

set (CMAKE_CXX_STANDARD 17)
set (GCC_COMPILER_FLAGS -ggdb -g -fpermissive)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
project(aflat VERSION 1.1.0)

file(GLOB_RECURSE lsp_SRC
    src/lsp/*.cpp
)
file(GLOB_RECURSE core_SRC
    src/*.cpp
)
list(REMOVE_ITEM core_SRC ${lsp_SRC})
list(REMOVE_ITEM core_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(aflat_core ${core_SRC})
target_include_directories(aflat_core PUBLIC include/)

# add boost
find_package(Boost 1.71.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(${GCC_COMPILER_FLAGS})

add_executable(aflat src/main.cpp)
target_link_libraries(aflat PRIVATE aflat_core)
target_include_directories(aflat PRIVATE include/)

add_executable(aflat-lsp ${lsp_SRC})
target_link_libraries(aflat-lsp PRIVATE aflat_core)
target_include_directories(aflat-lsp PRIVATE include/)

# Testing -->
file(GLOB_RECURSE testing_Modules
    test/*.cpp
    src/lsp/*.cpp
)
list(REMOVE_ITEM testing_Modules ${CMAKE_CURRENT_SOURCE_DIR}/src/lsp/main.cpp)

add_executable(test ${testing_Modules})
target_include_directories(test PRIVATE include/)
target_link_libraries(test PRIVATE aflat_core Catch2::Catch2WithMain)
