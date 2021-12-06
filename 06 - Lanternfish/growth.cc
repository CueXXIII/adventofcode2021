#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

void age(auto &lanternfish) {
  const auto spawns = lanternfish[0];
  for (auto i = 0; i < 8; ++i) {
    lanternfish[i] = lanternfish[i + 1];
  }
  lanternfish[8] = spawns;
  lanternfish[6] += spawns;
}

uint64_t count(auto &lanternfish) {
  uint64_t sum = 0;
  for (auto i = 0; i < 9; ++i) {
    sum += lanternfish[i];
  }
  return sum;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <ages.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string ages;
  infile >> ages;

  std::array<uint64_t, 9> lanternfish{0};

  for (auto c : ages) {
    if (c >= '0' && c <= '9') {
      lanternfish.at(c - '0') += 1;
    }
  }

  for (auto n = 0; n < 18; n++) {
    age(lanternfish);
  }
  std::cout << "There are " << count(lanternfish)
            << " lanternfish after 18 days.\n";

  for (auto n = 18; n < 80; n++) {
    age(lanternfish);
  }
  std::cout << "There are " << count(lanternfish)
            << " lanternfish after 80 days.\n";

  for (auto n = 80; n < 256; n++) {
    age(lanternfish);
  }
  std::cout << "There are " << count(lanternfish)
            << " lanternfish after 256 days.\n";
}
