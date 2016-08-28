project(libulib)

cmake_minimum_required(VERSION 3.0)

add_library(ulib ulib/bitset.c ulib/cache.c ulib/hash.c ulib/log.c
            ulib/options.c ulib/pgalloc.c ulib/rand.c ulib/time.c
            ulib/utf8.c ulib/vector.c)

link_libraries(ulib)
include_directories(${CMAKE_SOURCE_DIR})          

add_executable(test-pgalloc test/test-pgalloc.c)
add_executable(test-cache test/test-cache.c)
add_executable(test-splay-tree test/test-splay-tree.c)
add_executable(test-splay-tree-gc test/test-splay-tree-gc.c)
add_executable(test-avl-tree test/test-avl-tree.c)
add_executable(test-bitset test/test-bitset.c)
add_executable(test-options test/test-options.c)
add_executable(test-vector-regression test/test-vector-regression.c)
