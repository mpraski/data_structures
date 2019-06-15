//
// Created by marcin on 6/15/19.
//

#ifndef DATA_STRUCTURES_STRING_FNV_H
#define DATA_STRUCTURES_STRING_FNV_H

#include <algorithm>
#include <string>

class string_fnv {
public:
    std::size_t operator()(const std::string &s) const noexcept;

private:
    static const std::size_t Seed = 0x811C9DC5;
    static const std::size_t Prime = 0x01000193;

    std::size_t fnv1a(const void *data, std::size_t sz, std::size_t hash = Seed) const noexcept;
};


#endif //DATA_STRUCTURES_STRING_FNV_H
