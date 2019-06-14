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

template<class T, class... Fs>
class bloom_filter {
    template<typename B, typename ...As>
    using are_same = std::conjunction<std::is_same<B, As>...>;

    template<std::size_t N, typename... As>
    using nth_type = typename std::tuple_element<N, std::tuple<As...>>::type;

    static_assert(sizeof...(Fs), "You must supply at least one hash function");
    static_assert((std::is_default_constructible_v<Fs> && ...), "Each hash function must be default constructible");
    static_assert(are_same<nth_type<0, Fs...>, Fs...>::value, "Each hash function must be of the same type");

public:
    explicit bloom_filter(std::size_t size = 1u << 16u)
        : buffer_bits{size * CHAR_BIT},
          buffer{new unsigned char[size]},
          funcs{} {
      memset(buffer.get(), 0, size);
    }

    template<class I>
    void add(I &&item) noexcept {
      static_for(funcs, [&](const auto &f) {
        std::size_t hash{f(std::forward<I>(item)) % buffer_bits};
        buffer.get()[hash / 8] |= 1u << hash % CHAR_BIT;
      });
    }

    template<class I>
    bool has(I &&item) noexcept {
      return static_and(funcs, [&](const auto &f) {
        std::size_t hash{f(std::forward<I>(item)) % buffer_bits};
        return buffer.get()[hash / 8] & (1u << hash % CHAR_BIT);
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
      static_for_impl(t, std::forward<Func>(f), std::make_index_sequence<sizeof...(A)>{});
    }

    template<class Tup, class Func, std::size_t ...Is>
    constexpr static bool static_and_impl(Tup &&t, Func &&f, std::index_sequence<Is...>) {
      return (f(std::get<Is>(t)) && ...);
    }

    template<class... A, class Func>
    constexpr static bool static_and(const std::tuple<A...> &t, Func &&f) {
      return static_and_impl(t, std::forward<Func>(f), std::make_index_sequence<sizeof...(A)>{});
    }
};


#endif //DATA_STRUCTURES_BLOOM_FILTER_H
