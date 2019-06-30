//
// Created by marcin on 6/29/19.
//

#include "rsq_filter_block.h"

rsq_filter_block::rsq_filter_block()
    : offset{0},
      occupieds{0},
      runends{0},
      remainders(64) {
  std::fill(std::begin(remainders), std::end(remainders), 0ul);
}