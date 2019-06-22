//
// Created by marcin on 6/15/19.
//

#ifndef DATA_STRUCTURES_IMMUTABLE_BLOOMIER_FILTER_H
#define DATA_STRUCTURES_IMMUTABLE_BLOOMIER_FILTER_H

#include <cstddef>
#include <climits>
#include <array>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <bitset>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <assert.h>
#include "hasher.h"
#include "order_and_match.h"
#include "order_and_match_finder.h"

template<class T, class V, std::size_t M, std::size_t K>
class immutable_bloomier_filter {
    static_assert(std::is_default_constructible_v<V>, "V must be default constructible");

    const constexpr static auto Q = sizeof(V);
public:
    explicit immutable_bloomier_filter(const std::unordered_map<T, V> &items)
        : hash_seed{},
          hash{123211},
          data{new unsigned char[M * Q]},
          neighbourhood{},
          buffer{} {
      memset(data.get(), 0, M * Q);

      std::vector<T> keys;
      keys.reserve(items.size());
      for (const auto&[t, v] : items) {
        keys.push_back(t);
      }

      order_and_match_finder<T, V, M, K> oamf;
      if (auto oam{oamf.find(std::begin(keys), std::end(keys))}; oam) {
        create(items, *oam);
      } else {
        throw std::runtime_error("you fucked up");
      }
    }

    V get(const T &key) {
      hash.neighbourhood(neighbourhood, key);
      hash.m(buffer, key);

      auto *start_it{data.get()};
      static_for<0, K>([&](auto i) {
        array_xor(buffer, start_it + neighbourhood[i]);
      });

      return from_bytes<V>(std::begin(buffer), std::end(buffer));
    }

private:
    uint32_t hash_seed;
    hasher<T, M, K, Q> hash;
    std::unique_ptr<unsigned char> data;
    std::array<std::size_t, K> neighbourhood;
    std::array<unsigned char, Q> buffer;

    void create(const std::unordered_map<T, V> &items, const order_and_match<T> &oam) {
      hasher<T, M, K, Q> h{oam.seed};
      auto &pi{oam.pi};
      auto &tau{oam.tau};
      std::array<unsigned char, Q> temp_storage;

      for (std::size_t i{0}; i < pi.size(); ++i) {
        if (auto value{items.find(pi[i])}; value != items.end()) {
          h.neighbourhood(neighbourhood, value->second);
          h.m(buffer, value->second);

          auto index_of_storage{neighbourhood[tau[i]]};

          to_bytes(std::begin(temp_storage), value->second);
          array_xor(temp_storage, buffer);
          for (auto ha : neighbourhood) {
            array_xor(temp_storage, data.get() + ha);
          }

          std::copy(std::begin(temp_storage), std::end(temp_storage), data.get() + index_of_storage);
        }

      }
    }

    template<std::size_t N, class Iterator>
    inline void array_xor(
        Iterator start_one,
        Iterator start_two
    ) const {
      static_for<0, N>([&](auto i) {
        *start_one++ ^= *start_two++;
      });
    }

    template<class I, std::size_t N, class Iterator>
    inline void array_xor(
        std::array<I, N> &result,
        Iterator start
    ) const {
      static_assert(std::is_arithmetic_v<I>, "I must support arithmetic operations");
      static_for<0, N>([&](auto i) {
        result[i] ^= *start++;
      });
    }

    template<class I, std::size_t N>
    inline void array_xor(
        std::array<I, N> &result,
        std::array<I, N> &operand
    ) const {
      static_assert(std::is_arithmetic_v<I>, "I must support arithmetic operations");
      static_for<0, N>([&](auto i) {
        result[i] ^= operand[i];
      });
    }

    template<class I, class Iterator>
    inline I from_bytes(Iterator start, Iterator end) const {
      static_assert(std::is_trivially_copyable_v<I>, "I must be trivially copyable");
      assert(std::distance(start, end) == sizeof(I));
      I item;
      std::copy(start, end, reinterpret_cast<unsigned char *>(std::addressof(item)));
      return item;
    }

    template<class I, class Iterator>
    inline void to_bytes(Iterator start, const I &item) const {
      auto *start_i = reinterpret_cast<const unsigned char *>(std::addressof(item));
      auto *end_i = start_i + sizeof(I);
      std::copy(start_i, end_i, start);
    }

    template<std::size_t First, std::size_t Last, class F>
    inline void static_for(F &&f) const {
      if constexpr (First < Last) {
        f(std::integral_constant<std::size_t, First>{});
        static_for<First + 1, Last>(std::forward<F>(f));
      }
    }

};

#endif //DATA_STRUCTURES_IMMUTABLE_BLOOMIER_FILTER_H
