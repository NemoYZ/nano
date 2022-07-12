cmake_minimum_required(VERSION 2.8)

project(nano)

set(CMAKE_CXX_FLAGS "-Og -ggdb -std=c++20 -Wno-deprecated -Werror -Wall -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(allocator_test tests/allocator_test.cc)
#add_executable(heap_algo_test tests/heap_algo_test.cc)
add_executable(avl_tree_test tests/avl_tree_test.cc)
add_executable(skip_list_test tests/skip_list_test.cc)
add_executable(b_tree_test tests/b_tree_test.cc)
add_executable(algorithm_test tests/algorithm_test.cc)
add_executable(mini_vector_test tests/mini_vector_test.cc)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)