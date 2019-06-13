//
// Created by marcin on 6/13/19.
//

#ifndef DATA_STRUCTURES_VEB_TREE_H
#define DATA_STRUCTURES_VEB_TREE_H

#include <cstddef>
#include <climits>
#include <array>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

template<class T, std::size_t UB = sizeof(T) * CHAR_BIT>
class veb_tree {
    static_assert(std::is_integral_v<T>, "T must be an integral type");

    const static constexpr auto HALF_BITS = UB / 2;
    const static constexpr auto NONE = std::numeric_limits<T>::max();

    using child_t = veb_tree<T, HALF_BITS>;
    using child_ptr_t = std::unique_ptr<child_t>;
public:
    constexpr veb_tree() : min{NONE}, max{}, summary{}, clusters{} {}

    void insert(T value) {
      if (empty()) {
        min = max = value;
        return;
      }

      if (value < min) {
        std::swap(value, min);
      } else if (value > max) {
        max = value;
      }

      if constexpr (UB > 1) {
        T lo{low(value)};
        T hi{high(value)};

        if (!clusters[hi]) {
          clusters[hi] = std::make_unique<child_t>();
          insert_summary(hi);
        }

        clusters[hi]->insert(lo);
      }
    }

    T predecessor(T value) {
      if (value > max) {
        return max;
      }

      if (!summary) {
        if (value > min) {
          return min;
        }

        return NONE;
      }

      T lo{low(value)};
      T hi{high(value)};
      T lo2, hi2 = hi;

      if (lo > child_min(clusters[hi])) {
        if ((lo2 = clusters[hi]->predecessor(lo)) == NONE) {
          return NONE;
        }
      } else {
        if ((hi2 = summary->predecessor(hi)) == NONE) {
          if (min < value) {
            return min;
          }

          return NONE;
        }
        if ((lo2 = child_max(clusters[hi2])) == NONE) {
          return NONE;
        }
      }

      return merge(lo2, hi2);
    }

    template<class Iterator, class Inserter>
    void sort_descending(Iterator start, Iterator end, Inserter inserter) {
      for (auto it = start; it != end; it++) {
        insert(*it);
      }

      T pred = max;
      *inserter = max;
      while ((pred = predecessor(pred)) != NONE) {
        *inserter = pred;
      }
    }

    inline T getMin() const noexcept {
      return min;
    }

    inline T getMax() const noexcept {
      return max;
    }

private:
    T min, max;
    child_ptr_t summary;
    std::array<child_ptr_t, 1u << HALF_BITS> clusters;

    inline void insert_summary(T value) {
      if (!summary) {
        summary = std::make_unique<child_t>();
      }

      summary->insert(value);
    }

    inline bool empty() const noexcept {
      return min == NONE;
    }

    inline T child_min(const child_ptr_t &child) {
      return child ? child->getMin() : NONE;
    }

    inline T child_max(const child_ptr_t &child) {
      return child ? child->getMax() : NONE;
    }

    inline T low(T val) {
      return ((1u << HALF_BITS) - 1u) & val;
    }

    inline T high(T val) {
      return val >> HALF_BITS;
    }

    inline T merge(T low, T high) {
      return (high << HALF_BITS) + low;
    }
};

#endif //DATA_STRUCTURES_VEB_TREE_H
