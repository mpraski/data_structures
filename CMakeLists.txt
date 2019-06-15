cmake_minimum_required(VERSION 3.14)
project(data_structures)

set(CMAKE_CXX_STANDARD 17)

add_executable(data_structures main.cpp b_tree/b_tree_node.h b_tree/b_tree.h veb_tree/veb_tree.h bloom_filter/bloom_filter.h bloom_filter/hash/vendor/MurmurHash2.h bloom_filter/hash/vendor/MurmurHash2.cpp bloom_filter/hash/string_murmur.cpp bloom_filter/hash/string_murmur.h bloom_filter/hash/string_fnv.cpp bloom_filter/hash/string_fnv.h bloom_filter/bloomier_filter.h)