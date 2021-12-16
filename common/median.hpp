#pragma once

#include <algorithm>

namespace aoc {

// if the collection is even, it returns the upper bound of all medians
// since AOC answers are unique, the median range always should contain
// only one value.
auto median(auto &&collection) {
  const auto medianPosition =
      collection.begin() +
      static_cast<
          std::remove_reference<decltype(collection)>::type::difference_type>(
          collection.size()) /
          2;
  std::nth_element(collection.begin(), medianPosition, collection.end());
  return *medianPosition;
}

// the same for an iterator range
auto median(auto &&begin, auto &&end) {
  const auto medianPosition = begin + (end - begin) / 2;
  std::nth_element(begin, medianPosition, end);
  return *medianPosition;
}

} // namespace aoc
