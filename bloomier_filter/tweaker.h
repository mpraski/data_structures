//
// Created by marcin on 6/17/19.
//

#ifndef DATA_STRUCTURES_TWEAKER_H
#define DATA_STRUCTURES_TWEAKER_H

#include <cstddef>
#include <unordered_set>
#include <optional>
#include "hasher.h"

template<class T, std::size_t M, std::size_t K, std::size_t Q>
class tweaker {
public:
    template<class Iterator>
    tweaker(
        std::size_t seed,
        Iterator start,
        Iterator end
    ) : hash{seed}, non_singletons{} {
      std::unordered_set<std::size_t> hashes_seen;

      for (auto it = start; it != end; it++) {
        hash.neighbourhood(neighbourhood, *it);

        for (auto h : neighbourhood) {
          if (hashes_seen.find(h) != hashes_seen.end()) {
            non_singletons.insert(h);
          }
        }

        for (auto h : neighbourhood) {
          hashes_seen.insert(h);
        }
      }
    }

    std::optional<std::size_t> tweak(const T &key) noexcept {
      hash.neighbourhood(neighbourhood, key);
      for (std::size_t i{0}; i < K; ++i) {
        if (non_singletons.find(neighbourhood[i]) == non_singletons.end()) {
          return i;
        }
      }
      return {};
    }

private:
    hasher<T, M, K, Q> hash;
    std::unordered_set<std::size_t> non_singletons;
    std::array<std::size_t, K> neighbourhood;
};

#endif //DATA_STRUCTURES_TWEAKER_H
