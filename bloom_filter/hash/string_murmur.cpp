//
// Created by marcin on 6/15/19.
//

#include "string_murmur.h"

std::size_t string_murmur::operator()(const std::string &s) const noexcept {
  return MurmurHash2(s.c_str(), s.length(), 0);
}