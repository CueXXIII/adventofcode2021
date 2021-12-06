#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string_view>

constexpr void age(auto &lanternfish) {
  const auto spawns = lanternfish[0];
  std::copy(lanternfish.begin() + 1, lanternfish.end(), lanternfish.begin());
  lanternfish[8] = spawns;
  lanternfish[6] += spawns;
}

constexpr std::array<uint64_t, 9>
ageDays(const std::array<uint64_t, 9> &initial, const int days) {
  std::array<uint64_t, 9> lanternfish{initial};
  for (auto n = 0; n < days; ++n) {
    age(lanternfish);
  }
  return lanternfish;
}

constexpr uint64_t count(const auto &lanternfish) {
  return std::accumulate(lanternfish.begin(), lanternfish.end(), uint64_t{0});
}

constexpr std::array<uint64_t, 9> parseAges(const std::string_view &ages) {
  std::array<uint64_t, 9> lanternfish{0};
  for (auto c : ages) {
    if (c >= '0' && c <= '8') {
      lanternfish.at(c - '0') += 1;
    }
  }
  return lanternfish;
}

int main() {
  static constexpr std::string_view ages{
      "1,1,3,5,1,1,1,4,1,5,1,1,1,1,1,1,1,3,1,1,1,1,2,5,1,1,1,1,1,2,1,4,1,4,1,1,"
      "1,1,1,3,1,1,5,1,1,1,4,1,1,1,4,1,1,3,5,1,1,1,1,4,1,5,4,1,1,2,3,2,1,1,1,1,"
      "1,1,1,1,1,1,1,1,1,5,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,5,1,1,1,3,4,1,1,1,1,3,"
      "1,1,1,1,1,4,1,1,3,1,1,3,1,1,1,1,1,3,1,5,2,3,1,2,3,1,1,2,1,2,4,5,1,5,1,4,"
      "1,1,1,1,2,1,5,1,1,1,1,1,5,1,1,3,1,1,1,1,1,1,4,1,2,1,1,1,1,1,1,1,1,1,1,1,"
      "1,1,3,2,1,1,1,1,2,2,1,2,1,1,1,5,5,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,4,2,1,"
      "4,1,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,5,1,1,1,1,1,1,1,1,"
      "3,1,1,3,3,1,1,1,3,5,1,1,4,1,1,1,1,1,4,1,1,3,1,1,1,1,1,1,1,1,2,1,5,1,1,1,"
      "1,1,1,1,1,1,1,4,1,1,1,1"};

  static constexpr std::array<uint64_t, 9> lanternfishPart1{
      ageDays(parseAges(ages), 80)};
  static constexpr std::array<uint64_t, 9> lanternfishPart2{
      ageDays(lanternfishPart1, 256 - 80)};

  std::cout << "There are " << count(lanternfishPart1)
            << " lanternfish after 80 days.\n";

  std::cout << "There are " << count(lanternfishPart2)
            << " lanternfish after 256 days.\n";
}
