#pragma once

#include <algorithm>

namespace aoc {

// if the collection is odd, it returns the lower bound of all medians
// since AOC answers are unique, the median range always should contain
// only one element.
auto median(auto &&collection) {
  const auto medianPosition = collection.begin() + collection.size() / 2;
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
