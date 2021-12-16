#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <data.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};

}
