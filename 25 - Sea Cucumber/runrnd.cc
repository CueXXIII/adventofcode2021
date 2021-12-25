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
  SeaFloor(size_t width, size_t height)
      : width(width), height(height), stepCount(0) {
    const double p = 0.27;
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_real_distribution<double> choose(0.0, 1.0);

    floor.clear();
    //  bool horiz=true;
    for ([[maybe_unused]] const auto i : iota(size_t{0}, width * height)) {
      const auto roll = choose(e);
      if (roll < p) {
        floor.push_back('>');
      } else if (roll < 2 * p) {
        floor.push_back('v');
      } else {
        floor.push_back('.');
      }
      /*    if (roll < 2*p) {
              if(horiz) {
              floor.push_back('>');
              horiz=false;
              } else {
              floor.push_back('v');
              horiz=true;
              }
            } else {
              floor.push_back('.');
            } */
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
            /* if (floor[pos((x + 2) % width, y)] == '.') {
              next[pos(x, y)] = '.';
              next[pos((x + 2) % width, y)] = '>';
              hasMoved = true;
            } else */
            {
              next[pos(x, y)] = '.';
              next[pos((x + 1) % width, y)] = '>';
              hasMoved = true;
            }
          }
        }
      }
    }
    if (hasMoved) {
      floor = next;
    }
    for (const size_t y : iota(size_t{0}, height)) {
      for (const size_t x : iota(size_t{0}, width)) {
        if (floor[pos(x, y)] == 'v') {
          if (floor[pos(x, (y + 1) % height)] == '.') {
            /* if (floor[pos(x, (y + 2) % height)] == '.') {
              next[pos(x, y)] = '.';
              next[pos(x, (y + 2) % height)] = 'v';
              hasMoved = true;
            } else */
            {
              next[pos(x, y)] = '.';
              next[pos(x, (y + 1) % height)] = 'v';
              hasMoved = true;
            }
          }
        }
      }
    }
    if (hasMoved) {
      floor = std::move(next);
    }
    ++stepCount;
    return hasMoved;
  }

  void print() const {
    std::cout << "\x1b[1;1H";
    enum { none, red = 31, green = 32, yellow = 33 } color = none;
    for (const size_t y : iota(size_t{0}, height)) {
      for (const size_t x : iota(size_t{0}, width)) {
        switch (floor[pos(x, y)]) {
        case '.':
          std::cout << ' ';
          break;
        case '>':
          if (color != green) {
            std::cout << "\x1b[32m";
            color = green;
          }
          std::cout << '#'; // █
          break;
        case 'v':
          if (color != yellow) {

            std::cout << "\x1b[33m";
            color = yellow;
          }
          std::cout << '#';
          break;
        }
      }
      if (y < height - 1) {
        std::cout << '\n';
      }
    }
    // std::cout << "\x1b[!p";
    // std::cout << fmt::format("\nStep {:d}", stepCount) << std::flush;
  }
};

int main() {
  while (true) {
    std::cout << "\x1b[H\x1b[2J\x1b[3J";

    // The size of the grid is tuned to xterm using the "unreadble" font in
    // fullscreen on hd resolution.

    // SeaFloor ground{319,82}; // default
    // SeaFloor ground{1916, 538}; // unreadable
    // SeaFloor ground{383, 134}; // tiny
    SeaFloor ground{1915, 538}; // coprime <= unreadable

    while (true) {
      ground.print();
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(15ms); // 40ms = 25 fps
      // std::ranges::find_if(iota(1, 2000), [&ground](auto) { return
      // !ground.step(); });
      if (!ground.step()) {
        break;
      }
    }
    ground.print();
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(5s);
    // std::cout << "\n\n";
  }
}
