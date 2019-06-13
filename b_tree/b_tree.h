//
// Created by marcin on 6/12/19.
//

#ifndef DATA_STRUCTURES_B_TREE_H
#define DATA_STRUCTURES_B_TREE_H

#include <cstddef>
#include <climits>
#include <array>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

#include "b_tree_node.h"

template<class T, std::size_t N>
class b_tree {
public:
    b_tree() : root{allocate_node()} {}

    ~b_tree() {
      delete root;
    }

    void insert(const T &value) noexcept {
      if (root->full()) {
        auto old_root{root};
        root = new_root(root);

        insert_split(old_root, *root, 0);
      }

      insert_non_full(value, root);
    }

    void print_root() const noexcept {
      root->print(std::cout);
    }

private:
    b_tree_node<T, N> *root;

    inline auto allocate_node(bool is_leaf = true) {
      return new b_tree_node<T, N>(is_leaf);
    }

    inline auto new_root(b_tree_node<T, N> *const old_root) {
      auto *r = allocate_node(false);
      r->nodes[0] = old_root;
      return r;
    }

    inline auto next_index(const T &value, b_tree_node<T, N> &node) noexcept {
      std::size_t idx;
      for (idx = 0; idx < node.keys_n; ++idx) {
        if (value < node[idx]) break;
      }
      return idx;
    }

    void insert_non_full(const T &value, b_tree_node<T, N> *node) noexcept {
      auto *next{node};
      auto idx{next_index(value, *next)};

      while (!next->leaf()) {
        if (next->nodes[idx]->full()) {
          insert_split(next->nodes[idx], *next, idx);
        }
        next = next->nodes[idx];
        idx = next_index(value, *next);
      }

      next->shift_keys(idx);
      (*next)[idx] = value;
      next->keys_n++;
    }

    void insert_split(
        b_tree_node<T, N> *child,
        b_tree_node<T, N> &parent,
        std::size_t idx
    ) noexcept {
      auto &node{*child};
      auto median_idx{node.keys_n / 2};
      auto median_key{std::move(node[median_idx])};

      auto *right_child{b_tree_node<T, N>::create_right_child(node, median_idx)};
      node.become_left_child(median_idx);

      parent.shift(idx);
      parent[idx] = std::move(median_key);
      parent.keys_n++;
      parent.nodes[idx] = child;
      parent.nodes[idx + 1] = right_child;
    }
};


#endif //DATA_STRUCTURES_B_TREE_H
