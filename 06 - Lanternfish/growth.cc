#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>

void age(auto &lanternfish) {
  const auto spawns = lanternfish[0];
  std::copy(lanternfish.begin() + 1, lanternfish.end(), lanternfish.begin());
  lanternfish[8] = spawns;
  lanternfish[6] += spawns;
}

uint64_t count(const auto &lanternfish) {
  return std::accumulate(lanternfish.begin(), lanternfish.end(), uint64_t{0});
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
    if (c >= '0' && c <= '8') {
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
