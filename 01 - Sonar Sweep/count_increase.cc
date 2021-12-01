#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <measurements.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  int depth, prevdepth;
  int count{0};
  infile >> prevdepth;
  while (infile >> depth) {
    if (depth > prevdepth) {
      ++count;
    }
    prevdepth = depth;
  }
  std::cout << "Measured " << count << " increases.\n";
}
