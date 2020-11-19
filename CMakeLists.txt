cmake_minimum_required(VERSION 3.17)
project(MaxMatching)
enable_testing()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror -fno-omit-frame-pointer -DDEBUG_OUTPUT")

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SOURCES
        src/graph.h src/graph.cpp src/matching.cpp src/matching.h
        src/nested_shrinking.cpp src/nested_shrinking.h src/alternating_tree.cpp
        src/alternating_tree.h src/perfect_matching_algorithm.cpp src/perfect_matching_algorithm.h src/representative_vector.h src/representative.h)

add_executable(MaxMatching src/main.cpp ${COMMON_SOURCES} src/maximum_matching_algorithm.cpp src/maximum_matching_algorithm.h)
