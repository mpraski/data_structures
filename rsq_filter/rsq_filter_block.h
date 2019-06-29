//
// Created by marcin on 6/29/19.
//

#ifndef DATA_STRUCTURES_RSQ_FILTER_BLOCK_H
#define DATA_STRUCTURES_RSQ_FILTER_BLOCK_H

#include <cstddef>
#include <cstdint>
#include <vector>

class rsq_filter_block {
public:
    uint8_t offset;
    uint64_t occupieds;
    uint64_t runends;
    std::vector<uint64_t> remainders;

    rsq_filter_block();
};


#endif //DATA_STRUCTURES_RSQ_FILTER_BLOCK_H
