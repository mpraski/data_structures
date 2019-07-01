cmake_minimum_required(VERSION 3.14)
project(data_structures)

set(CMAKE_CXX_STANDARD 17)

add_executable(data_structures main.cpp b_tree/b_tree_node.h b_tree/b_tree.h veb_tree/veb_tree.h bloom_filter/bloom_filter.h vendor/MurmurHash2.h vendor/MurmurHash2.cpp bloom_filter/hash/string_murmur.cpp bloom_filter/hash/string_murmur.h bloom_filter/hash/string_fnv.cpp bloom_filter/hash/string_fnv.h bloomier_filter/immutable_bloomier_filter.h vendor/MurmurHash3.cpp vendor/MurmurHash3.h bloomier_filter/hasher.h bloomier_filter/order_and_match.h bloomier_filter/tweaker.h bloomier_filter/order_and_match_finder.h rsq_filter/rsq_filter.h)

target_compile_options (data_structures PRIVATE -mbmi2)