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
#include <algorithm>
#include <numeric>

#if defined(__BMI2__)

#include <immintrin.h>
#include <random>

#endif

#include "../vendor/MurmurHash3.h"

#define bit(n) (1ul << (n))
#define DEFAULTS_SLOTS (1u << 8u)
#define DEFAULT_BLOCK_SLOTS 64u

constexpr bool is_prime(size_t n) noexcept {
  if (n == 2u || n == 3u)
    return true;
  if (n == 1u || n % 2u == 0u)
    return false;
  for (size_t i{3u}; i * i <= n; i += 2u)
    if (n % i == 0u)
      return false;
  return true;
}

constexpr size_t next_prime(size_t n) noexcept {
  if (!n)
    n = 3;
  else if (n % 2 == 0)
    n++;
  for (; !is_prime(n); n += 2);
  return n;
}

template<class T, size_t SLOTS = DEFAULTS_SLOTS, size_t BLOCK_SLOTS = DEFAULT_BLOCK_SLOTS>
class rsq_filter {
    const constexpr static auto BLOCKS = next_prime(SLOTS * SLOTS);
    const constexpr static auto MAGIC_NUMBER = 0xAAAAAAAA;

    static_assert(BLOCKS < std::numeric_limits<uint32_t>::max());
public:
    rsq_filter()
        : blocks{(rsqf_block *) calloc(BLOCKS, sizeof(rsqf_block))},
          metadata{},
          hashes{},
          free_list_pointer{},
          seed{} {
      // Initialize free list
      for (uint32_t i{0u}; i < BLOCKS - 1; ++i) {
        metadata[i].next_free = i + 1;
        metadata[i + 1].prev_free = i;
      }

      auto engine_seed{std::chrono::system_clock::now().time_since_epoch().count()};
      std::mt19937 rng(engine_seed);
      std::uniform_int_distribution<uint32_t> random(
          std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()
      );
      seed = random(rng);
    }

    bool may_contain(const T &item) {
      hash(item);
      auto quotient{hashes[0]};
      auto remainder{hashes[1]};
      auto idx{block_idx(quotient)};
      auto rem{block_rem(quotient)};

      const auto *block{block_at(idx)};
      if (!is_set(block->occupieds, rem)) {
        return false;
      }

      auto l{rank_select(*block, rem)};
      if (!l) {
        return block->remainders[rem] == remainder;
      }

      do {
        if (block->remainders[l--] == remainder) {
          return true;
        }
      } while (l >= rem && !is_set(block->runends, l));

      return false;
    }

    void insert(const T &item) {
      hash(item);
      auto quotient{hashes[0]};
      auto remainder{hashes[1]};
      auto idx{block_idx(quotient)};
      auto rem{block_rem(quotient)};

      auto *block{block_at(idx)};

      auto s{rank_select(*block, rem)};
      if (!block->occupieds || rem > s) {
        block->remainders[rem] = remainder;
        set(block->runends, rem);
      } else {
        s++;
        auto n{first_unused_slot(*block, s)};
        while (n > s) {
          block->remainders[n] = block->remainders[n - 1];
          copy_bit(block->runends, n, n - 1);
          n--;
        }
        block->remainders[s] = remainder;
        if (is_set(block->occupieds, rem)) {
          clear(block->runends, s - 1);
        }
        set(block->runends, s);
      }

      set(block->occupieds, rem);
    }

private:
    struct rsqf_block {
        uint8_t offset;
        uint64_t occupieds;
        uint64_t runends;
        uint64_t remainders[BLOCK_SLOTS];
    };

    struct rsqf_block_metadata {
        uint32_t prev_free;
        uint32_t next_free;
        uint32_t next_logical;
        uint32_t hash;
    } __attribute__((packed));

    std::unique_ptr<rsqf_block> blocks;
    std::array<rsqf_block_metadata, BLOCKS> metadata;
    std::array<uint64_t, 2> hashes;
    uint32_t free_list_pointer;
    uint32_t seed;

    inline void hash(const T &key) {
      MurmurHash3_x64_128(std::addressof(key), sizeof(T), seed, hashes.data());
    }

    inline bool is_set(uint64_t vec, unsigned i) const noexcept {
      return vec & bit(i);
    }

    inline void set(uint64_t &vec, unsigned i) const noexcept {
      vec |= bit(i);
    }

    inline void set(uint8_t &vec, unsigned i) const noexcept {
      vec |= bit(i);
    }

    inline void clear(uint64_t &vec, unsigned i) const noexcept {
      vec &= ~bit(i);
    }

    inline void copy_bit(uint64_t &vec, unsigned a, unsigned b) const noexcept {
      (vec & bit(b)) ? set(vec, a) : clear(vec, a);
    }

    inline unsigned rank(uint64_t vec, unsigned i) const noexcept {
      return __builtin_popcountll(vec & (bit(i) - 1ul));
    }

    inline unsigned select(uint64_t vec, unsigned i) const noexcept {
      return __builtin_clzll(_pdep_u64(bit(i), vec));
    }

    inline unsigned rank_select(const rsqf_block &block, unsigned i) const noexcept {
      if (auto r{rank(block.occupieds, i)}; r) {
        return select(block.runends, r);
      }
      return 0u;
    }

    unsigned first_unused_slot(const rsqf_block &block, unsigned i) const noexcept {
      auto s{rank_select(block, i)};

      while (i <= s) {
        i = s + 1;
        s = rank_select(block, i);
      }

      return i;
    }

    inline uint32_t low(uint64_t val) const noexcept {
      return (bit(32ul) - 1ul) & val;
    }

    inline uint32_t high(uint64_t val) const noexcept {
      return val >> 32ul;
    }

    inline uint8_t block_rem(uint64_t quotient) const noexcept {
      return quotient % BLOCK_SLOTS;
    }

    auto block_idx(uint64_t quotient) {
      quotient /= BLOCK_SLOTS;
      auto lo{low(quotient) % BLOCKS};
      auto hi{high(quotient)};
      auto &meta{metadata[lo]};
      uint32_t collisions{1u};

      while (meta.hash && meta.hash != hi) {
        lo = lo + 2 * ++collisions - 1;
        if (lo >= BLOCKS) {
          lo -= BLOCKS;
        }
      }

      if (!meta.hash) {
        meta.hash = hi;
        if (lo == free_list_pointer) {
          increment_free_list_pointer();
        } else {
          metadata[meta.prev_free].next_free = meta.next_free;
          metadata[meta.next_free].prev_free = meta.prev_free;
        }
      }

      return lo;
    }

    auto next_block_idx(uint32_t block_idx) {
      if (!metadata[block_idx].next_logical) {
        metadata[block_idx].next_logical = free_list_pointer;
        metadata[free_list_pointer].hash = MAGIC_NUMBER;
        increment_free_list_pointer();
      }
      return metadata[block_idx].next_logical;
    }

    inline auto *block_at(uint32_t block_idx) {
      return blocks.get() + block_idx;
    }

    inline void increment_free_list_pointer() {
      free_list_pointer = metadata[free_list_pointer].next_free;
    }
};


#endif //DATA_STRUCTURES_RSQ_FILTER_H
