#include "all.hpp"

struct Vec2 {
  int64_t x;
  int64_t y;

  Vec2 operator+(const Vec2 &other) const {
    return Vec2{x + other.x, y + other.y};
  }
};

class CornGrid {
  Vec2 size;
  std::vector<bool> grid;
  bool voidVal{false};

private:
  size_t index(const Vec2 &pos) const {
    return static_cast<size_t>(pos.x + size.x * pos.y);
  }
  bool isValidPos(const Vec2 &pos) const {
    return (pos.x >= 0 && pos.x < size.x) && (pos.y >= 0 && pos.y < size.y);
  }

  int64_t valueNeighbours(const Vec2 &pos) {
    int64_t value = 0;
    for (const int64_t y : {-1, 0, 1}) {
      for (const int64_t x : {-1, 0, 1}) {
        const Vec2 neighbour{pos + Vec2{x, y}};
        value <<= 1;
        if (isValidPos(neighbour)) {
          value |= grid[index(neighbour)];
        } else {
          value |= voidVal;
        }
      }
    }
    return value;
  }

public:
  CornGrid(const Vec2 &size) : size(size) {
    grid.resize(static_cast<size_t>(size.x * size.y), false);
  }
  CornGrid(std::ifstream &in) {
    std::vector<std::string> field;
    std::string line;
    while (in >> line) {
      field.push_back(line);
    }
    size = {static_cast<int64_t>(field[0].size()),
            static_cast<int64_t>(field.size())};
    grid.resize(static_cast<size_t>(size.x * size.y), false);

    for (int64_t x = 0; x < size.x; ++x) {
      for (int64_t y = 0; y < size.y; ++y) {
        grid[index({x, y})] =
            (field[static_cast<size_t>(y)][static_cast<size_t>(x)] == '#');
      }
    }
  }

  void set(const Vec2 &pos) { grid.at(index(pos)) = true; }

  int64_t countPixels() const {
    return std::count_if(grid.begin(), grid.end(),
                         [](auto val) { return val; }) *
           (voidVal ? -1 : 1);
  }

  void step(const std::vector<bool> &rules) {
    CornGrid next{size + Vec2{2, 2}};
    for (int64_t x = -1; x <= size.x; ++x) {
      for (int64_t y = -1; y <= size.y; ++y) {
        const int64_t value{valueNeighbours({x, y})};
        next.grid[next.index(Vec2{x, y} + Vec2{1, 1})] =
            rules[static_cast<size_t>(value)];
      }
    }
    // TODO: prune grid? Nah, didn't do it in 2020/17 either
    grid = std::move(next.grid);
    size = std::move(next.size);
    voidVal = rules[voidVal ? 511 : 0];
  }

  void print() const {
    for (int64_t y = 0; y < size.y; ++y) {
      for (int64_t x = 0; x < size.x; ++x) {
        std::cout << (grid[index({x, y})] ? '#' : '.');
      }
      std::cout << '\n';
    }
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <raw.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;
  std::getline(infile, line);
  std::vector<bool> rules;
  for (const auto bit : line) {
    rules.push_back(bit == '#');
  }
  std::getline(infile, line);
  CornGrid image{infile};

  std::cout << fmt::format("Initially there are {:d} pixel\n",
                           image.countPixels());
  image.step(rules);
  std::cout << fmt::format("After  1 step  there {:d} pixel\n",
                           image.countPixels());
  image.step(rules);
  std::cout << fmt::format("After  2 steps there {:d} pixel\n",
                           image.countPixels());
}
