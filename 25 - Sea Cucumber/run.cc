#include "all.hpp"

using std::views::iota;

class SeaFloor {
  size_t width;
  size_t height;
  int stepCount;
  std::vector<char> floor;

private:
  size_t pos(const size_t x, const size_t y) const { return x + y * width; }

public:
  SeaFloor(std::ifstream &in) : width(0), height(0), stepCount(0) {
    std::string line;
    while (std::getline(in, line)) {
      if (width == 0) {
        width = line.size();
      }
      ++height;
      for (const auto &c : line) {
        floor.push_back(c);
      }
    }
  }

  bool step() {
    bool hasMoved = false;
    std::vector<char> next;
    next = floor;
    for (const size_t y : iota(size_t{0}, height)) {
      for (const size_t x : iota(size_t{0}, width)) {
        if (floor[pos(x, y)] == '>') {
          if (floor[pos((x + 1) % width, y)] == '.') {
            next[pos(x, y)] = '.';
            next[pos((x + 1) % width, y)] = '>';
            hasMoved = true;
          }
        }
      }
    }
    floor = std::move(next);

    next = floor;
    for (const size_t y : iota(size_t{0}, height)) {
      for (const size_t x : iota(size_t{0}, width)) {
        if (floor[pos(x, y)] == 'v') {
          if (floor[pos(x, (y + 1) % height)] == '.') {
            next[pos(x, y)] = '.';
            next[pos(x, (y + 1) % height)] = 'v';
            hasMoved = true;
          }
        }
      }
    }
    floor = std::move(next);
    ++stepCount;
    return hasMoved;
  }

  void print() const {
    std::cout << "\x1b[1;1H";
    for (const size_t y : iota(size_t{0}, height)) {
      for (const size_t x : iota(size_t{0}, width)) {
        std::cout << floor[pos(x, y)];
      }
      std::cout << '\n';
    }
    std::cout << fmt::format("\nStep {:d}", stepCount) << std::flush;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <floor.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::cout << "\x1b[H\x1b[2J\x1b[3J";

  std::ifstream infile{argv[1]};
  SeaFloor ground{infile};

  while (true) {
    ground.print();
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(40ms); // 40ms = 25 fps
    if (!ground.step()) {
      break;
    }
  }
  ground.print();
  std::cout << "\n\n";
}
