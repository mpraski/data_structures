//
// Created by marcin on 6/15/19.
//

#include "string_fnv.h"

std::size_t string_fnv::operator()(const std::string &s) const noexcept {
  return fnv1a(s.c_str(), s.length());
}

std::size_t string_fnv::fnv1a(const void *data, std::size_t sz, std::size_t hash) const noexcept {
  auto *ptr = (const unsigned char *) data;
  while (sz--) hash = (*ptr++ ^ hash) * Prime;
  return hash;
}