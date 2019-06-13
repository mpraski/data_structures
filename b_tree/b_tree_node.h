//
// Created by marcin on 6/12/19.
//

#ifndef DATA_STRUCTURES_B_TREE_NODE_H
#define DATA_STRUCTURES_B_TREE_NODE_H

#include <cstddef>
#include <climits>
#include <array>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

template<class T, std::size_t N>
class b_tree;

template<class T, std::size_t N>
class b_tree_node {
    using key_iterator_t = typename std::array<T, N>::iterator;
    using node_iterator_t = typename std::array<b_tree_node<T, N> *, N>::const_iterator;
public:
    explicit b_tree_node(bool is_leaf = true) noexcept : keys_n{}, keys{}, nodes{}, is_leaf{is_leaf} {}

    b_tree_node(
        key_iterator_t key_start,
        key_iterator_t key_end,
        node_iterator_t node_start,
        node_iterator_t node_end,
        bool leaf
    ) noexcept : keys_n{static_cast<std::size_t>(std::distance(key_start, key_end))},
                 keys{},
                 nodes{},
                 is_leaf{leaf} {
      std::copy(
          std::make_move_iterator(key_start),
          std::make_move_iterator(key_end),
          std::begin(keys)
      );
      std::copy(
          node_start,
          node_end,
          std::begin(nodes)
      );
    }

    ~b_tree_node() {
      for (auto node : nodes)
        if (node)
          delete node;
    }

    static auto create_right_child(b_tree_node<T, N> &parent, std::size_t idx) {
      return new b_tree_node<T, N>{
          std::next(std::begin(parent.keys), idx + 1),
          std::end(parent.keys),
          std::next(std::cbegin(parent.nodes), idx + 1),
          std::cend(parent.nodes),
          parent.leaf()
      };
    }

    T &operator[](std::size_t idx) {
      return keys[idx];
    }

    inline bool empty() const noexcept {
      return !keys_n;
    }

    inline bool full() const noexcept {
      return keys_n == N;
    }

    inline bool leaf() const noexcept {
      return is_leaf;
    }

    inline void become_left_child(std::size_t idx) noexcept {
      keys_n = idx;
      std::fill(
          std::next(std::begin(nodes), idx + 1),
          std::end(nodes),
          nullptr
      );
    }

    inline void shift(std::size_t idx) noexcept {
      if (!empty()) {
        shift_container(keys, idx);
        shift_container(nodes, idx);
      }
    }

    inline void shift_keys(std::size_t idx) noexcept {
      if (!empty()) {
        shift_container(keys, idx);
      }
    }

    void print(std::ostream &os, std::size_t indent = 0) const noexcept {
      do_indent(os, indent);
      os << "Keys: ";

      for (std::size_t i = 0; i < keys_n; ++i) {
        os << keys[i] << ", ";
      }

      os << std::endl;
      do_indent(os, indent);
      os << "Nodes:" << std::endl;

      bool has{false};
      for (const auto *node : nodes) {
        if (node) {
          node->print(os, indent + 2);
          has = true;
        }
      }

      if (!has) {
        do_indent(os, indent + 2);
        os << "(None)" << std::endl;
      }
    }

private:
    std::size_t keys_n;
    std::array<T, N> keys;
    std::array<b_tree_node<T, N> *, N> nodes;
    bool is_leaf;

    friend class b_tree<T, N>;

    inline void do_indent(std::ostream &os, std::size_t val) const noexcept {
      std::fill_n(std::ostream_iterator<char>(os), val, ' ');
    }

    template<class Container>
    inline void shift_container(Container &container, std::size_t idx) {
      std::copy(
          std::next(std::begin(container), idx),
          std::end(container),
          std::next(std::begin(container), idx + 1)
      );
    }
};

#endif //DATA_STRUCTURES_B_TREE_NODE_H
