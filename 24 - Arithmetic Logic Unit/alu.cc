#include "all.hpp"

struct Param {
  const int a;
  const int b;
  const int c;
};

// The MONAD consists of 14 almost identical passes, only 3 parameters differ
static constexpr std::array<Param, 14> MP{{{1, 14, 12},
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

// ASIC for all 14 ALU rounds
constexpr int64_t round(const size_t step, const int64_t z, const int64_t w) {
  const bool px = ((z % 26 + MP[step - 1].b) != w);
  const int64_t py = 25 * px + 1;
  const int64_t zp = (z / MP[step - 1].a) * py + (w + MP[step - 1].c) * px;
  return zp;
}

// limit for z is the product of the left param.a values
// hard coded: 26^7 till step 7, then 26^(14-step)
constexpr bool zInLimit(const size_t step, const int64_t z) {
  if (step <= 7) {
    return z < 8031810176;
  }
  switch (step) {
  case 8:
    return z < 308915776;
  case 9:
    return z < 11881376;
  case 10:
    return z < 456976;
  case 11:
    return z < 17576;
  case 12:
    return z < 676;
  case 13:
    return z < 26;
  case 14:
    return z < 1;
  }
  return false;
}

// sID: steps done so far
// mID: partial machine ID
// z: value of register z after sID steps
// prints out all valid machine IDs
constexpr void recurse(const size_t sID, const int64_t mID, const int64_t z) {
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
