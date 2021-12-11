#include <cstdlib>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "digit_field.hpp"

size_t substep(auto &&field, auto &&flashing, const size_t width,
               const size_t height) {
  size_t flashSeen = 0;
  for (size_t y = 1; y < height - 1; ++y) {
    for (size_t x = 1; x < width - 1; ++x) {
      if (!flashing[x + y * width] && field[x + y * width] > 9) {
        flashSeen += 1;
        flashing[x + y * width] = true;
        for (const auto dx : {-1, 0, 1}) {
          for (const auto dy : {-1, 0, 1}) {
            field[(x + dx) + (y + dy) * width] += 1;
          }
        }
      }
    }
  }
  return flashSeen;
}

size_t step(auto &&field, const size_t width, const size_t height) {
  size_t flashes, flashesTotal = 0;
  for (auto &cell : field) {
    cell += 1;
  }
  std::vector<bool> flashing;
  flashing.resize(width * height, false);
  while ((flashes = substep(field, flashing, width, height)) > 0) {
    flashesTotal += flashes;
  }
  for (auto &cell : field) {
    if (cell > 9) {
      cell = 0;
    }
  }
  return flashesTotal;
}

void printField(auto &&field, size_t width, size_t height) {
  for (size_t y = 1; y < height - 1; ++y) {
    for (size_t x = 1; x < width - 1; ++x) {
      std::cout << static_cast<char>(field[x + y * width] + '0');
    }
    std::cout << "\n";
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <octopuses.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::vector<uint8_t> field;
  size_t width, height;
  std::tie(width, height) = aoc::parseDigitField(infile, field);

  size_t totalFlashes = 0;
  for (auto i = 0; i < 100; ++i) {
    totalFlashes += step(field, width, height);
  }
  std::cout << "After 100 steps, you have seen " << totalFlashes
            << " flashes\n";

  // TODO: what if the octopuses are synchronized before step 101?
  size_t stepNo = 100;
  for (;;) {
    ++stepNo;
    if (step(field, width, height) == 100) {
      std::cout << "In step " << stepNo
                << " all 100 octopuses flash simultaneously.\n";
      break;
    }
  }
}
