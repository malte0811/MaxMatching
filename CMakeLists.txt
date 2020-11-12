cmake_minimum_required(VERSION 3.17)
project(MaxMatching)
enable_testing()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror")

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SOURCES src/graph.hpp src/graph.cpp src/union_find.cpp src/union_find.hpp src/matching.cpp src/matching.h src/nested_shrunken_graph.cpp src/nested_shrunken_graph.h test/ShrinkerTest.cpp)

add_executable(MaxMatching src/main.cpp ${COMMON_SOURCES})

set(Boost_ADDITIONAL_VERSIONS "1.67" "1.67.0")

# finds boost, triggers an error otherwise
find_package(Boost 1.67 REQUIRED COMPONENTS unit_test_framework)

# indicates the location of the boost installation tree.
# hard-coded for our simple example.
set(BOOST_INCLUDE_DIRS /etc/include)
add_executable(Test test/MatchingTest.cpp ${COMMON_SOURCES} test/TestMain.cpp)

target_include_directories(Test PRIVATE ${BOOST_INCLUDE_DIRS})
target_compile_definitions(Test PRIVATE "BOOST_TEST_DYN_LINK=1")
target_link_libraries(Test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# declares a test with our executable
add_test(NAME test1 COMMAND Test)