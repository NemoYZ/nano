cmake_minimum_required(VERSION 3.0)

project(nano)

string(CONCAT CXX_FLAGS
    "-rdynamic " 
    "-Og "
    "-ggdb "
    "-std=c++20 "
    "-Wno-deprecated "
    "-Werror "
    "-Wall "
    "-Wno-unused-function "
    "-Wno-builtin-macro-redefined"
)

set(LIB_SRC
    ${PROJECT_SOURCE_DIR}/src/tree.cc
    ${PROJECT_SOURCE_DIR}/src/hash.cc
)

add_library(nano SHARED ${LIB_SRC})

set(CMAKE_CXX_FLAGS ${CXX_FLAGS})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(heap_algo_test tests/heap_algo_test.cc)

add_executable(avl_tree_test tests/avl_tree_test.cc)
target_link_libraries(avl_tree_test nano)

add_executable(skip_list_test tests/skip_list_test.cc)

add_executable(b_tree_test tests/b_tree_test.cc)
target_link_libraries(b_tree_test nano)

add_executable(algorithm_test tests/algorithm_test.cc)
add_executable(mini_vector_test tests/mini_vector_test.cc)

add_executable(hash_table_test tests/hash_table_test.cc)
target_link_libraries(hash_table_test nano)

add_executable(rb_tree_test tests/rb_tree_test.cc)
target_link_libraries(rb_tree_test nano)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)