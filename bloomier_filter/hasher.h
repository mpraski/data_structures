//
// Created by marcin on 6/17/19.
//

#ifndef DATA_STRUCTURES_HASHER_H
#define DATA_STRUCTURES_HASHER_H

#include <cstddef>
#include <array>
#include "../vendor/MurmurHash3.h"

template<class T, std::size_t M, std::size_t K, std::size_t Q>
class hasher {
public:
    explicit hasher(std::size_t seed) : seed{seed} {}

    void neighbourhood(std::array<std::size_t, K> &buffer, const T &key) const noexcept {
      static uint64_t hash_output[2];

      std::size_t inc{0};
      for (std::size_t i{0}; i < K; ++i) {
        if (!(i % 2)) {
          MurmurHash3_x64_128(std::addressof(key), sizeof(T), seed + (inc++), hash_output);
        }

        buffer[i] = hash_output[i % 2] % M;
      }
    }

    void m(std::array<unsigned char, Q> &buffer, const T &key) const noexcept {
      constexpr auto bytes{2 * sizeof(uint64_t)};
      static unsigned char hash_output[bytes];

      std::size_t inc{0};
      for (std::size_t i{0}; i < Q; ++i) {
        if (!(i % bytes)) {
          MurmurHash3_x64_128(std::addressof(key), sizeof(T), seed + (inc++), hash_output);
        }

        buffer[i] = hash_output[i % bytes];
      }
    }

    void set_seed(std::size_t s) noexcept {
      seed = s;
    }

private:
    std::size_t seed;

};

#endif //DATA_STRUCTURES_HASHER_H
