//
// Created by marcin on 6/17/19.
//

#ifndef DATA_STRUCTURES_ORDER_AND_MATCH_H
#define DATA_STRUCTURES_ORDER_AND_MATCH_H

#include <cstddef>
#include <vector>

template<class T>
struct order_and_match {
    std::size_t seed;
    std::vector<T> pi;
    std::vector<std::size_t> tau;
};

#endif //DATA_STRUCTURES_ORDER_AND_MATCH_H
