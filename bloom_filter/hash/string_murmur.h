//
// Created by marcin on 6/15/19.
//

#ifndef DATA_STRUCTURES_STRING_MURMUR_H
#define DATA_STRUCTURES_STRING_MURMUR_H

#include <string>
#include "vendor/MurmurHash2.h"

class string_murmur {
public:
    std::size_t operator()(const std::string &s) const noexcept;
};

#endif //DATA_STRUCTURES_STRING_MURMUR_H
