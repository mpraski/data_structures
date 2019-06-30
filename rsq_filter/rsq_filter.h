//
// Created by marcin on 6/29/19.
//

#ifndef DATA_STRUCTURES_RSQ_FILTER_H
#define DATA_STRUCTURES_RSQ_FILTER_H

#include <cstddef>
#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <chrono>

#if defined(__BMI2__)

#include <immintrin.h>
#include <random>

#endif

#include "rsq_filter_block.h"
#include "../vendor/MurmurHash3.h"

#define bit(n) (1ul << (n))

template<class T>
class rsq_filter {
public:
    rsq_filter() : blocks{}, hashes{}, seed{} {
      auto engine_seed{std::chrono::system_clock::now().time_since_epoch().count()};
      std::mt19937 rng(engine_seed);
      std::uniform_int_distribution<std::size_t> random(
          std::numeric_limits<std::size_t>::min(),
          std::numeric_limits<std::size_t>::max()
      );
      seed = random(rng);
    }

    bool may_contain(const T &item) {
      hash(item);
      auto quotient{hashes[0]};
      auto remainder{hashes[1]};
      auto idx{block_idx(quotient)};
      auto rem{block_rem(quotient)};

      const auto &block{get_block(idx)};
      if (!is_set(block.occupieds, rem)) {
        return false;
      }

      auto l{rank_select(block, rem)};
      if (!l) {
        return block.remainders[rem] == remainder;
      }

      do {
        if (block.remainders[l--] == remainder) {
          return true;
        }
      } while (l >= rem && !is_set(block.runends, l));

      return false;
    }

    void insert(const T &item) {
      hash(item);
      auto quotient{hashes[0]};
      auto remainder{hashes[1]};
      auto idx{block_idx(quotient)};
      auto rem{block_rem(quotient)};

      auto &block{get_block(idx)};

      auto s{rank_select(block, rem)};
      if (!block.occupieds || rem > s) {
        block.remainders[rem] = remainder;
        set(block.runends, rem);
      } else {
        s++;
        auto n{first_unused_slot(block, s)};
        while (n > s) {
          block.remainders[n] = block.remainders[n - 1];
          copy_bit(block.runends, n, n - 1);
          n--;
        }
        block.remainders[s] = remainder;
        if (is_set(block.occupieds, rem)) {
          clear(block.runends, s - 1);
        }
        set(block.runends, s);
      }

      set(block.occupieds, rem);
    }

private:
    std::unordered_map<uint64_t, rsq_filter_block> blocks;
    std::array<uint64_t, 2> hashes;
    std::size_t seed;

    inline void hash(const T &key) {
      MurmurHash3_x64_128(std::addressof(key), sizeof(T), seed, hashes.data());
    }

    inline bool is_set(uint64_t vec, unsigned i) const noexcept {
      return vec & bit(i);
    }

    inline void set(uint64_t &vec, unsigned i) const noexcept {
      vec |= bit(i);
    }

    inline void clear(uint64_t &vec, unsigned i) const noexcept {
      vec &= ~bit(i);
    }

    inline void copy_bit(uint64_t &vec, unsigned a, unsigned b) const noexcept {
      (vec & bit(b)) ? set(vec, a) : clear(vec, a);
    }

    inline uint64_t block_idx(uint64_t quotient) const noexcept {
      return quotient / 64ul;
    }

    inline uint64_t block_rem(uint64_t quotient) const noexcept {
      return quotient % 64ul;
    }

    inline unsigned rank(uint64_t vec, unsigned i) const noexcept {
      return __builtin_popcountl(vec & (bit(i) - 1ul));
    }

    inline unsigned select(uint64_t vec, unsigned i) const noexcept {
      return __builtin_clzl(_pdep_u64(bit(i), vec));
    }

    inline unsigned rank_select(const rsq_filter_block &block, unsigned i) const noexcept {
      if (auto r{rank(block.occupieds, i)}; r) {
        return select(block.runends, r);
      }
      return 0u;
    }

    inline auto &get_block(uint64_t quotient) {
      if (auto it{blocks.find(quotient)}; it == blocks.end()) {
        blocks.emplace(quotient, rsq_filter_block{});
      }
      return blocks[quotient];
    }

    unsigned first_unused_slot(const rsq_filter_block &block, unsigned i) const noexcept {
      auto s{rank_select(block, i)};

      while (i <= s) {
        i = s + 1;
        s = rank_select(block, i);
      }

      return i;
    }
};


#endif //DATA_STRUCTURES_RSQ_FILTER_H
