//
// Created by marcin on 6/14/19.
//

#ifndef DATA_STRUCTURES_BLOOM_FILTER_H
#define DATA_STRUCTURES_BLOOM_FILTER_H

#include <cstddef>
#include <climits>
#include <array>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <bitset>
#include <cstring>

#include "hash/string_murmur.h"
#include "hash/string_fnv.h"

template<class... Fs>
class bloom_filter {
    static_assert(sizeof...(Fs), "You must supply at least one hash function");
    static_assert((std::is_default_constructible_v<Fs> && ...), "Each hash function must be default constructible");

    const constexpr static auto COUNTER_WIDTH = CHAR_BIT / 2u;
    const constexpr static auto COUNTER_SIZE = 1u << COUNTER_WIDTH;
    const constexpr static auto LO_MASK = (1u << COUNTER_WIDTH) - 1;
    const constexpr static auto HI_MASK = LO_MASK << COUNTER_WIDTH;
public:
    explicit bloom_filter(std::size_t size = 1u << 16u)
        : buffer_bits{size * CHAR_BIT},
          buffer{new unsigned char[size * COUNTER_WIDTH]},
          funcs{} {
      memset(buffer.get(), 0, size * COUNTER_WIDTH);
    }

    template<class I>
    void add(I &&item) noexcept {
      static_for(funcs, [&](const auto &f) {
        std::size_t hash{f(std::forward<I>(item)) % buffer_bits};
        increment_counter(buffer.get()[hash / 2], (hash % 2) ? COUNTER_WIDTH : 0);
      });
    }

    template<class I>
    void remove(I &&item) noexcept {
      static_for(funcs, [&](const auto &f) {
        std::size_t hash{f(std::forward<I>(item)) % buffer_bits};
        if (auto &bits{buffer.get()[hash / 2]}; extract(bits, hash % 2 == 0)) {
          decrement_counter(bits, (hash % 2) ? COUNTER_WIDTH : 0);
        }
      });
    }

    template<class I>
    bool maybe_has(I &&item) noexcept {
      return static_and(funcs, [&](const auto &f) {
        std::size_t hash{f(std::forward<I>(item)) % buffer_bits};
        return extract(buffer.get()[hash / 2], hash % 2 == 0);
      });
    }

private:
    std::size_t buffer_bits;
    std::unique_ptr<unsigned char> buffer;
    std::tuple<Fs...> funcs;

    template<class Tup, class Func, std::size_t ...Is>
    constexpr static void static_for_impl(Tup &&t, Func &&f, std::index_sequence<Is...>) {
      (f(std::get<Is>(t)), ...);
    }

    template<class... A, class Func>
    constexpr static void static_for(const std::tuple<A...> &t, Func &&f) {
      static_for_impl(t, std::forward<Func>(f), std::index_sequence_for<A...>{});
    }

    template<class Tup, class Func, std::size_t ...Is>
    constexpr static bool static_and_impl(Tup &&t, Func &&f, std::index_sequence<Is...>) {
      return (f(std::get<Is>(t)) && ...);
    }

    template<class... A, class Func>
    constexpr static bool static_and(const std::tuple<A...> &t, Func &&f) {
      return static_and_impl(t, std::forward<Func>(f), std::index_sequence_for<A...>{});
    }

    inline auto extract(unsigned char bits, bool pos) const noexcept {
      return pos ? (bits & HI_MASK) >> COUNTER_WIDTH : bits & LO_MASK;
    }

    inline void increment_counter(unsigned char &counter, unsigned offset) const noexcept {
      unsigned m{1u}, n{m << offset};
      while (m < COUNTER_SIZE && counter & n) {
        counter ^= n;
        m <<= 1u;
        n <<= 1u;
      }
      if (m != COUNTER_SIZE) {
        counter ^= n;
      }
    }

    inline void decrement_counter(unsigned char &counter, unsigned offset) const noexcept {
      unsigned m{1u}, n{m << offset};
      while (m < COUNTER_SIZE && counter ^ n) {
        counter |= n;
        m <<= 1u;
        n <<= 1u;
      }
      if (m != COUNTER_SIZE) {
        counter ^= n;
      }
    }
};

using string_bloom = bloom_filter<string_murmur, string_fnv>;

#endif //DATA_STRUCTURES_BLOOM_FILTER_H
