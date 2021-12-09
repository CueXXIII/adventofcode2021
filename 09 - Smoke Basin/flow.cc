#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

class Vec2 {
public:
  const size_t x;
  const size_t y;
};

class HeightMap {
private:
  size_t width{0};
  size_t height{0};
  std::vector<uint8_t> map;

  std::vector<Vec2> lowPoints;

public:
  uint8_t &at(const size_t x, const size_t y) { return map[x + y * width]; }

  uint8_t &at(const Vec2 &pos) { return map[pos.x + pos.y * width]; }

private:
  void findLowPoints() {
    for (size_t y = 1; y < height - 1; ++y) {
      for (size_t x = 1; x < width - 1; ++x) {
        if (at(x, y) < at(x - 1, y) && at(x, y) < at(x + 1, y)) {
          if (at(x, y) < at(x, y - 1) && at(x, y) < at(x, y + 1)) {
            lowPoints.push_back({x, y});
          }
        }
      }
    }
  }

public:
  size_t totalRiskLevel() {
    return std::accumulate(lowPoints.begin(), lowPoints.end(), 0,
                           [this](auto a, auto b) { return a + 1 + at(b); });
  }

  friend std::ifstream &operator>>(std::ifstream &in, HeightMap &me) {
    std::string line;
    while (in >> line) {
      if (me.width == 0) {
        me.width = line.size() + 2;
        for (size_t i = 0; i < me.width; ++i) {
          me.map.push_back(9);
        }
      }
      ++me.height;
      me.map.push_back(9);
      for (const auto cell : line) {
        me.map.push_back(static_cast<uint8_t>(cell - '0'));
      }
      me.map.push_back(9);
    }
    for (size_t i = 0; i < me.width; ++i) {
      me.map.push_back(9);
    }
    me.height += 2;
    me.findLowPoints();
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, HeightMap &me) {
    for (size_t y = 0; y < me.height; ++y) {
      for (size_t x = 0; x < me.width; ++x) {
        out << static_cast<char>(me.at(x, y) + '0');
      }
      out << '\n';
    }
    out << '\n';
    out << "LowPoints = {";
    bool first = true;
    for (const auto pos : me.lowPoints) {
      if (!first) {
        out << ", ";
      }
      out << "(" << pos.x << ", " << pos.y << ")";
      first = false;
    }
    out << "}\n\n";
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <heightmap.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  HeightMap oceanFloor;

  std::ifstream infile{argv[1]};
  infile >> oceanFloor;

  // std::cout << oceanFloor;

  std::cout << "Total risk level = " << oceanFloor.totalRiskLevel() << ".\n";
}
