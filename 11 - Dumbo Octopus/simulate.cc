#include <cstdlib>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "digit_field.hpp"

class Cavern {
private:
  std::vector<uint8_t> field;
  size_t width;
  size_t height;

private:
  size_t pos(auto x, auto y) const {
    return static_cast<size_t>(x + y * static_cast<decltype(y)>(width));
  }

  size_t substep(auto &&flashing) {
    size_t flashSeen = 0;
    for (auto y = 1; std::cmp_less(y, height - 1); ++y) {
      for (auto x = 1; std::cmp_less(x, width - 1); ++x) {
        if (!flashing[pos(x, y)] && field[pos(x, y)] > 9) {
          flashSeen += 1;
          flashing[pos(x, y)] = true;
          for (const auto dx : {-1, 0, 1}) {
            for (const auto dy : {-1, 0, 1}) {
              field[pos(x - dx, y - dy)] += 1;
            }
          }
        }
      }
    }
    return flashSeen;
  }

public:
  size_t step() {
    size_t flashes, flashesTotal = 0;
    for (auto &cell : field) {
      cell += 1;
    }
    std::vector<bool> flashing;
    flashing.resize(width * height, false);
    while ((flashes = substep(flashing)) > 0) {
      flashesTotal += flashes;
    }
    for (auto &cell : field) {
      if (cell > 9) {
        cell = 0;
      }
    }
    return flashesTotal;
  }

  friend std::ostream &operator<<(std::ostream &out, Cavern &cavern) {
    for (size_t y = 1; y < cavern.height - 1; ++y) {
      for (size_t x = 1; x < cavern.width - 1; ++x) {
        out << static_cast<char>(cavern.field[cavern.pos(x, y)] + '0');
      }
      out << "\n";
    }
    return out;
  }

  friend std::ifstream &operator>>(std::ifstream &in, Cavern &cavern) {
    std::tie(cavern.width, cavern.height) =
        aoc::parseDigitField(in, cavern.field);
    return in;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <octopuses.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  Cavern cavern;
  std::ifstream infile{argv[1]};
  infile >> cavern;

  size_t totalFlashes = 0;
  for (auto i = 0; i < 100; ++i) {
    totalFlashes += cavern.step();
  }
  std::cout << "After 100 steps, you have seen " << totalFlashes
            << " flashes\n";

  // TODO: what if the octopuses are synchronized before step 101?
  size_t stepNo = 100;
  for (;;) {
    ++stepNo;
    if (cavern.step() == 100) {
      std::cout << "In step " << stepNo
                << " all 100 octopuses flash simultaneously.\n";
      break;
    }
  }
}
