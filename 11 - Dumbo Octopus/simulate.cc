#include <cstdlib>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "digit_field.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <octopuses.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::vector<uint8_t> field;
  size_t width, height;
  std::tie(width,height) = aoc::parseDigitField(infile, field);

}
