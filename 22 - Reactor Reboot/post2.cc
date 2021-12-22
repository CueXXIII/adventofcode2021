#include "all.hpp"

struct QuantCoord {
  std::set<int64_t> quants;
  std::vector<int64_t> qs;

public:
  void add(const int64_t coord) {
    quants.insert(coord);
    qs = {quants.begin(), quants.end()};
  }
};

struct Cuboid {
  int64_t xmin, xmax, ymin, ymax, zmin, zmax;
  bool state;

public:
  bool contains(const int64_t x, const int64_t y, const int64_t z) const {
    return x >= xmin and x <= xmax and y >= ymin and y <= ymax and z >= zmin and
           z <= zmax;
  }
};

class Core {
  std::vector<Cuboid> cuboids{};
  QuantCoord qx{};
  QuantCoord qy{};
  QuantCoord qz{};

public:
  void set(const int64_t xmin, const int64_t xmax, const int64_t ymin,
           const int64_t ymax, const int64_t zmin, const int64_t zmax,
           bool value) {
    cuboids.emplace_back(xmin, xmax, ymin, ymax, zmin, zmax, value);
    qx.add(xmin);
    qx.add(xmax + 1);
    qy.add(ymin);
    qy.add(ymax + 1);
    qz.add(zmin);
    qz.add(zmax + 1);
  }

  int64_t count() {
    int64_t count = 0;
    for (size_t xi = 0; xi < qx.qs.size() - 1; ++xi) {
      std::cout << 'x' << std::flush;
      const auto x = qx.qs[xi];
      const auto xn = qx.qs[xi + 1];
      for (size_t yi = 0; yi < qy.qs.size() - 1; ++yi) {
        const auto y = qy.qs[yi];
        const auto yn = qy.qs[yi + 1];
        for (size_t zi = 0; zi < qz.qs.size() - 1; ++zi) {
          const auto z = qz.qs[zi];
          const auto zn = qz.qs[zi + 1];
          bool isSet = false;
          for (const auto &cube : cuboids) {
            if (cube.contains(x, y, z)) {
              isSet = cube.state;
            }
          }
          if (isSet) {
            count += (xn - x) * (yn - y) * (zn - z);
          }
        }
      }
    }
    std::cout << "\n";
    return count;
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
  std::cout << fmt::format("After full POST there are {:d} cubes on.\n",
                           core.count());
}
