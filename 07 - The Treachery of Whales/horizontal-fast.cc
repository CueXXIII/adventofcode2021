#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#include "median.hpp"

class Positions : public std::vector<int16_t> {

public:
  int64_t fuelConsumption(int16_t destination, auto metric) const {
    return std::accumulate(this->begin(), this->end(), int64_t{0},
                           [destination, metric](auto a, auto b) {
                             return a + metric(destination, b);
                           });
  }
  friend std::ifstream &operator>>(std::ifstream &in, Positions &list) {
    char delim;
    do {
      int16_t pos;
      in >> pos;
      list.push_back(pos);
    } while (in >> delim && delim == ',');
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, Positions &list) {
    out << "Entries = " << list.size() << "\n";
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <positions.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  Positions positions;
  infile >> positions;

  const auto minDistPosition{aoc::median(positions)};
  const auto minDistFuel{positions.fuelConsumption(
      minDistPosition, [](auto a, auto b) { return std::abs(a - b); })};
  std::cout << "Found minimum at position " << minDistPosition << ",\n";
  std::cout << "The minimum distance to align horizontally is " << minDistFuel
            << ".\n";

  const auto averageLow{
      std::accumulate(positions.begin(), positions.end(), int64_t{0}) /
      static_cast<int64_t>(positions.size())};
  const auto averageHigh{averageLow + 1};

  const auto triangleSum{[](auto a, auto b) {
    const auto dist{std::abs(a - b)};
    return dist * (dist + 1) / 2;
  }};
  const auto averageLowFuel{
      positions.fuelConsumption(static_cast<int16_t>(averageLow), triangleSum)};
  const auto averageHighFuel{positions.fuelConsumption(
      static_cast<int16_t>(averageHigh), triangleSum)};
  if (averageLowFuel < averageHighFuel) {
    std::cout << "Found minimum at position " << averageLow << ",\n";
    std::cout << "The minimum fuel used to align horizontally is "
              << averageLowFuel << ".\n";
  } else {
    std::cout << "Found minimum at position " << averageHigh << ",\n";
    std::cout << "The minimum fuel used to align horizontally is "
              << averageHighFuel << ".\n";
  }
}
