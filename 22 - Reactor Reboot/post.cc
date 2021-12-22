#include "all.hpp"

class Core {
  std::vector<uint8_t> cuboids;
  uint8_t infinity;

public:
  Core() { cuboids.resize(101 * 101 * 101, false); }
  uint8_t &at(const int64_t x, const int64_t y, const int64_t z) {
    if (x >= -50 and x <= 50 and y >= -50 and y <= 50 and z >= -50 and
        z <= 50) {
      return cuboids[static_cast<size_t>((x + 50) + (y + 50) * 101 +
                                         (z + 50) * 101 * 101)];
    } else {
      return infinity;
    }
  }
  void set(const int64_t xmin, const int64_t xmax, const int64_t ymin,
           const int64_t ymax, const int64_t zmin, const int64_t zmax,
           bool value) {
    for (const int64_t x : std::views::iota(xmin, xmax + 1)) {
      for (const int64_t y : std::views::iota(ymin, ymax + 1)) {
        for (const int64_t z : std::views::iota(zmin, zmax + 1)) {
          at(x, y, z) = value;
        }
      }
    }
  }

  int64_t count() const {
    return std::ranges::count_if(cuboids, [](auto a) { return a; });
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <cubes.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  Core core{};

  std::ifstream infile{argv[1]};
  std::string line;
  const std::regex parser{"(on|off) "
                          "x=(-?[0-9]+)\\.\\.(-?[0-9]+),"
                          "y=(-?[0-9]+)\\.\\.(-?[0-9]+),"
                          "z=(-?[0-9]+)\\.\\.(-?[0-9]+)",
                          std::regex::extended};

  while (std::getline(infile, line)) {
    std::cout << "." << std::flush;
    std::smatch tokens;
    if (!std::regex_match(line, tokens, parser)) {
      std::cout << "Error on line: " << line << "\n";
      continue;
    }
    const bool op = (tokens[1] == "on");
    core.set(std::stol(tokens[2]), std::stol(tokens[3]), std::stol(tokens[4]),
             std::stol(tokens[5]), std::stol(tokens[6]), std::stol(tokens[7]),
             op);
  }
  std::cout << '\n';

  std::cout << fmt::format("After POST there are {:d} cubes on.\n",
                           core.count());
}
