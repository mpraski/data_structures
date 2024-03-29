#include <iostream>
#include "b_tree/b_tree.h"
#include "veb_tree/veb_tree.h"
#include "bloom_filter/bloom_filter.h"
#include "bloomier_filter/immutable_bloomier_filter.h"
#include "rsq_filter/rsq_filter.h"

#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>

int main() {
  /*std::cout << "Hello, World!" << std::endl;
  string_bloom filter;
  filter.add("lel");
  filter.add("lul");
  filter.remove("lel");

  std::cout << "contains lel: " << filter.maybe_has("lel") << std::endl;
  std::cout << "contains lul: " << filter.maybe_has("lul") << std::endl;

  immutable_bloomier_filter<int, int, 10, 10> ibf{{
                                                      {1, 2},
                                                      {2, 4}
                                                  }};

  std::cout << "found for 1: " << ibf.get(1) << std::endl;*/

  rsq_filter<int> rsqf;
  std::cout << "contains 1: " << rsqf.may_contain(1) << std::endl;
  rsqf.insert(1);
  std::cout << "contains 1: " << rsqf.may_contain(1) << std::endl;

  std::cout << "contains 2: " << rsqf.may_contain(2) << std::endl;
  rsqf.insert(2);
  std::cout << "contains 2: " << rsqf.may_contain(2) << std::endl;

  /*b_tree<int, 5> tree;
  tree.insert(10);
  tree.insert(9);
  tree.insert(8);
  tree.insert(7);
  tree.insert(6);
  tree.insert(5);
  tree.insert(4);
  tree.insert(3);
  tree.insert(2);
  tree.insert(11);
  tree.insert(12);
  tree.insert(13);
  tree.insert(14);
  tree.insert(15);
  tree.print_root();

  std::random_device rd;
  std::mt19937 g(rd());

  std::vector<unsigned int> nums(1000000);
  std::vector<unsigned int> sorted;
  std::iota(std::begin(nums), std::end(nums), 0);

  std::shuffle(std::begin(nums), std::end(nums), g);
  std::vector<unsigned int> nums_copy = nums;

  veb_tree<unsigned int> veb;
  auto start = std::chrono::steady_clock::now();
  veb.sort_descending(std::begin(nums), std::end(nums), std::back_inserter(sorted));
  auto end = std::chrono::steady_clock::now();
  auto tot = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "veb.sort_descending: "
            << tot
            << " us" << std::endl;

  auto start2 = std::chrono::steady_clock::now();
  std::sort(std::begin(nums_copy), std::end(nums_copy));
  auto end2 = std::chrono::steady_clock::now();
  auto tot2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();
  std::cout << "std::sort: "
            << tot2
            << " us" << std::endl;

  std::cout << "veb to sort ratio: " << tot / tot2 << std::endl;*/
  return 0;
}