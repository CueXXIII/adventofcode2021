#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

class Positions {
private:
  std::vector<int16_t> positions;

public:
  int16_t min;
  int16_t max;

public:
  int64_t fuelConsumption(int16_t destination, auto metric) const {
    return std::accumulate(positions.begin(), positions.end(), int64_t{0},
                           [destination, metric](auto a, auto b) {
                             return a + metric(destination, b);
                           });
  }
  friend std::ifstream &operator>>(std::ifstream &in, Positions &list) {
    int16_t pos;
    char delim;
    in >> pos;
    list.positions.push_back(pos);
    list.min = pos;
    list.max = pos;
    while (in >> delim && delim == ',') {
      in >> pos;
      list.positions.push_back(pos);
      if (list.min > pos) {
        list.min = pos;
      }
      if (list.max < pos) {
        list.max = pos;
      }
    }
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, Positions &list) {
    out << "Entries = " << list.positions.size() << ", Min = " << list.min
        << ", Max = " << list.max << "\n";
    return out;
  }
};

int64_t findMinimum(Positions &positions, auto metric) {
  int64_t minFuel{positions.fuelConsumption(positions.min, metric)};
  for (int16_t blast = positions.min + 1; blast <= positions.max; ++blast) {
    const auto currentFuel{positions.fuelConsumption(blast, metric)};
    if (currentFuel > minFuel) {
      std::cout << "Found minimum at position " << blast - 1 << ".\n";
      break;
    }
    minFuel = currentFuel;
  }
  return minFuel;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <positions.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  Positions positions;
  infile >> positions;

  const auto minDist =
      findMinimum(positions, [](auto a, auto b) { return std::abs(a - b); });
  std::cout << "The minimum distance to align horizontally is " << minDist
            << ".\n";

  const auto minFuel = findMinimum(positions, [](auto a, auto b) {
    const auto dist{std::abs(a - b)};
    return dist * (dist + 1) / 2;
  });
  std::cout << "The minimum fuel used to align horizontally is " << minFuel
            << ".\n";
}
