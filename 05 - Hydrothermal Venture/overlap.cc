#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

// 500 lines on 1000x1000 grid -> uint16 per pixel
class ParaxialOcean {
private:
  const size_t width = 10;
  const size_t height = 10;
  size_t overlaps = 0;
  std::vector<uint16_t> grid;

  size_t pos(const size_t x, const size_t y) const { return x + y * width; }
  void setPixel(const size_t x, const size_t y) {
    const auto position = pos(x, y);
    const auto value = grid[position];
    grid[position] = value + 1;
    if (value == 1) {
      overlaps += 1;
    }
  }

public:
  ParaxialOcean(const size_t width, const size_t height)
      : width(width), height(height), overlaps(0) {
    grid.resize(width * height);
  }

  void addLine(size_t x1, size_t y1, size_t x2, size_t y2) {
    if (y1 == y2) {
      // horizontal
      if (x2 < x1) {
        std::swap(x1, x2);
      }
      for (auto x = x1; x <= x2; ++x) {
        setPixel(x, y1);
      }
    } else if (x1 == x2) {
      // vertical
      if (y2 < y1) {
        std::swap(y1, y2);
      }
      for (auto y = y1; y <= y2; ++y) {
        setPixel(x1, y);
      }
    }
  }

  size_t countOverlap() const { return overlaps; }

  friend std::ifstream &operator>>(std::ifstream &in, ParaxialOcean &o) {
    std::string line;
    if (!std::getline(in, line)) {
      return in;
    }
    const char *cline = line.c_str();
    char *cend;
    const auto x1 = std::strtoul(cline, &cend, 10);
    if (cend[0] != ',') {
      in.close();
      return in;
    }
    cline = cend + 1;
    const auto y1 = std::strtoul(cline, &cend, 10);
    if (cend[0] != ' ' || cend[1] != '-' || cend[2] != '>' || cend[3] != ' ') {
      in.close();
      return in;
    }
    cline = cend + 4;
    const auto x2 = std::strtoul(cline, &cend, 10);
    if (cend[0] != ',') {
      in.close();
      return in;
    }
    cline = cend + 1;
    const auto y2 = std::strtoul(cline, &cend, 10);
    o.addLine(x1, y1, x2, y2);
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, const ParaxialOcean o) {
    for (auto y = 0u; y < o.height; ++y) {
      for (auto x = 0u; x < o.width; ++x) {
        const auto pixel = o.grid[o.pos(x, y)];
        if (pixel == 0) {
          out << '.';
        } else if (pixel < 10) {
          out << pixel;
        } else {
          out << '#';
        }
      }
      out << '\n';
    }
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <lines.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};

  ParaxialOcean ocean{1000, 1000};
  while (infile >> ocean)
    ;
  std::cout << "There are " << ocean.countOverlap()
            << " pixels with paraxial lines overlapping\n";
}
