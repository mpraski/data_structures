//
// Created by marcin on 6/20/19.
//

#ifndef DATA_STRUCTURES_ORDER_AND_MATCH_FINDER_H
#define DATA_STRUCTURES_ORDER_AND_MATCH_FINDER_H

#include <cstddef>
#include <vector>
#include <algorithm>
#include <numeric>
#include "tweaker.h"
#include "order_and_match.h"

template<class T, class V, std::size_t M, std::size_t K>
class order_and_match_finder {
    const constexpr static auto Q = sizeof(V);
    const constexpr static auto MAX_SIZE = std::numeric_limits<std::size_t>::max();
public:
    template<class Iterator>
    std::optional<order_and_match<T>> find(Iterator start, Iterator end) {
      hasher<T, M, K, Q> hash{seed};
      for (std::size_t i{0}; i < MAX_SIZE; ++i) {
        pi.clear();
        pi_temp.clear();
        tau.clear();

        std::copy(start, end, std::back_inserter(pi_temp));

        if (find_match(pi_temp)) {
          return order_and_match<T>{
              seed,
              std::move(pi),
              tau
          };
        }

        hash.set_seed(++seed);
      }
      return {};
    }

private:
    std::size_t seed;
    std::vector<T> pi;
    std::vector<T> pi_temp;
    std::vector<std::size_t> tau;

    bool find_match(std::vector<T> &remaining_keys) {
      if (remaining_keys.empty()) {
        return true;
      }

      auto rk_size{remaining_keys.size()};
      tweaker<T, M, K, Q> tweaker{seed, std::cbegin(remaining_keys), std::cend(remaining_keys)};

      remaining_keys.erase(
          std::remove_if(
              remaining_keys.begin(),
              remaining_keys.end(),
              [&](auto &key) {
                if (auto iota{tweaker.tweak(key)}; iota) {
                  pi.push_back(std::move(key));
                  tau.push_back(*iota);
                  return true;
                }

                return false;
              }
          ),
          remaining_keys.end()
      );

      if (rk_size == remaining_keys.size()) {
        return false;
      }

      if (!remaining_keys.empty()) {
        if (!find_match(remaining_keys)) {
          return false;
        }
      }

      return true;
    }
};

#endif //DATA_STRUCTURES_ORDER_AND_MATCH_FINDER_H
