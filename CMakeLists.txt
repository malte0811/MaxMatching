cmake_minimum_required(VERSION 3.17)
project(MaxMatching)
enable_testing()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror -fno-omit-frame-pointer")

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SOURCES
        src/graph.hpp src/graph.cpp src/union_find.cpp src/union_find.hpp src/matching.cpp src/matching.h
        src/nested_shrinking.cpp src/nested_shrinking.h src/alternating_tree.cpp
        src/alternating_tree.h src/perfect_matching_algorithm.cpp src/perfect_matching_algorithm.h)

add_executable(MaxMatching src/main.cpp ${COMMON_SOURCES} src/maximum_matching_algorithm.cpp src/maximum_matching_algorithm.h)

set(Boost_ADDITIONAL_VERSIONS "1.67" "1.67.0")

# finds boost, triggers an error otherwise
find_package(Boost 1.67 REQUIRED COMPONENTS unit_test_framework)

# indicates the location of the boost installation tree.
# hard-coded for our simple example.
set(BOOST_INCLUDE_DIRS /etc/include)
add_executable(Test test/ShrinkerTest.cpp test/MatchingTest.cpp test/TestMain.cpp ${COMMON_SOURCES})

target_include_directories(Test PRIVATE ${BOOST_INCLUDE_DIRS})
target_compile_definitions(Test PRIVATE "BOOST_TEST_DYN_LINK=1")
target_link_libraries(Test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# declares a test with our executable
add_test(NAME test1 COMMAND Test)