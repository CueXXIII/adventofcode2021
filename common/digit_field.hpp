#pragma once

#include <fstream>
#include <string>
#include <type_traits>
#include <utility>

namespace aoc {

auto parseDigitField(std::ifstream &in, auto &&field,
                     typename std::remove_reference<
                         decltype(field)>::type::value_type borderValue = 0) {
  size_t width = 0;
  size_t height = 0;
  std::string line;

  field.clear();

  while (in >> line) {
    if (width == 0) {
      width = line.size() + 2;
      ++height;
      for (size_t i = 0; i < width; ++i) {
        field.push_back(borderValue);
      }
    }

    ++height;
    field.push_back(borderValue);

    for (const auto cell : line) {
      field.push_back(static_cast<decltype(borderValue)>(cell - '0'));
    }
    field.push_back(borderValue);
  }
  ++height;
  for (size_t i = 0; i < width; ++i) {
    field.push_back(borderValue);
  }

  return std::pair{width, height};
}

} // namespace aoc
