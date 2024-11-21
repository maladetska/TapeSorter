cmake_minimum_required(VERSION 3.28)
project(TapeSorter
    VERSION 3.0
    DESCRIPTION "Tape structure with sort"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)
