#include "all.hpp"

struct Param {
  const int a;
  const int b;
  const int c;
};

// The MONAD consists of 14 almost identical passes, only 3 parameters differ
static constexpr std::array<const Param, 14> MP{{{1, 14, 12},
                                                 {1, 15, 7},
                                                 {1, 12, 1},
                                                 {1, 11, 2},
                                                 {26, -5, 4},
                                                 {1, 14, 15},
                                                 {1, 15, 11},
                                                 {26, -13, 5},
                                                 {26, -16, 3},
                                                 {26, -8, 9},
                                                 {1, 15, 2},
                                                 {26, -8, 3},
                                                 {26, 0, 3},
                                                 {26, -4, 11}}};

// limit for z is the product of the left param.a values
[[nodiscard]] consteval int64_t calcZLimit(size_t step) noexcept {
  return std::accumulate(MP.begin() + step + 1, MP.end(), int64_t{1},
                         [](auto a, auto b) { return a * b.a; });
};
static constexpr std::array<const int64_t, 14> zLimits{
    calcZLimit(0),  calcZLimit(1), calcZLimit(2),  calcZLimit(3),
    calcZLimit(4),  calcZLimit(5), calcZLimit(6),  calcZLimit(7),
    calcZLimit(8),  calcZLimit(9), calcZLimit(10), calcZLimit(11),
    calcZLimit(12), calcZLimit(13)};
[[nodiscard]] constexpr bool zInLimit(const size_t step,
                                      const int64_t z) noexcept {
  return z < zLimits[step - 1];
}

// ASIC for all 14 ALU rounds
[[nodiscard]] constexpr int64_t round(const size_t step, const int64_t z,
                                      const int64_t w) noexcept {
  const bool px = ((z % 26 + MP[step - 1].b) != w);
  const int64_t py = 25 * px + 1;
  const int64_t zp = (z / MP[step - 1].a) * py + (w + MP[step - 1].c) * px;
  return zp;
}

// sID: steps done so far
// mID: partial machine ID
// z: value of register z after sID steps
// prints out all valid machine IDs
constexpr void recurse(const size_t sID, const int64_t mID,
                       const int64_t z) noexcept {
  if (sID == 14) {
    if (z == 0) {
      std::cout << fmt::format("Machine ID {:d} is valid\n", mID);
    }
    return;
  }
  const auto step = sID + 1;
  for (const auto i : std::views::iota(1, 10)) {
    // search digits from the top
    const int64_t digit = 10 - i;
    const int64_t zp = round(step, z, digit);
    if (zInLimit(step, zp)) {
      recurse(step, mID * 10 + digit, zp);
    }
  }
}

int main() { recurse(0, 0, 0); }
