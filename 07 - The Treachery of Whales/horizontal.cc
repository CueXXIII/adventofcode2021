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
  int64_t fuelConsumption(int16_t destination) const {
    return std::accumulate(positions.begin(), positions.end(), int64_t{0},
                           [destination](auto a, auto b) {
                             return a + std::abs(destination - b);
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

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <positions.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  Positions positions;
  infile >> positions;

  int64_t minFuel{positions.fuelConsumption(positions.min)};
  for (int16_t blast = positions.min + 1; blast <= positions.max; ++blast) {
    const auto currentFuel{positions.fuelConsumption(blast)};
    if (currentFuel > minFuel) {
      break;
    }
    minFuel = currentFuel;
  }

  std::cout << "The minimum fuel to align horizontally is " << minFuel << ".\n";
}
