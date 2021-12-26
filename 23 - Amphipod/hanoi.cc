#include "all.hpp"

// Burrow positions
// 0 -- 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 -- 8 -- 9 -- 10
//           |         |         |         |
//          11        13        15        17
//           |         |         |         |
//          12        14        16        18

struct Path {
  std::vector<size_t> vertices{};
  int cost = 0;

  friend std::ostream &operator<<(std::ostream &out, const Path &p) {
    out << "(";
    bool first = true;
    for (const auto &v : p.vertices) {
      if (!first) {
        out << ", ";
      }
      out << fmt::format("{:2d}", v);
      first = false;
    }
    out << "), cost = " << p.cost;
    return out;
  }
};

static std::vector<Path> paths;

Path findPath(size_t src, const size_t dst) {
  Path path{};
  path.vertices.push_back(src);
  if (src == dst) {
    return path;
  }
  if (src > 10 && dst > 10 && ((src + 1) | 1) == ((dst + 1) | 1)) {
    path.vertices = {src, dst};
    path.cost = 1;
    return path;
  }
  if (src > 10) {
    if ((src & 1) == 0) {
      src -= 1;
      path.vertices.push_back(src);
      ++path.cost;
    }
    src -= 9;
    ++path.cost;
    Path section = findPath(src, dst);
    for (const auto v : section.vertices) {
      path.vertices.push_back(v);
    }
    path.cost += section.cost;
    return path;
  }
  if (dst > 10) {
    const size_t wayp = ((dst - 1) | 1) - 9;
    path = findPath(src, wayp);
    if ((dst & 1) == 0) {
      path.vertices.push_back(dst - 1);
      ++path.cost;
    }
    path.vertices.push_back(dst);
    ++path.cost;
    return path;
  }
  if (src < dst) {
    for (size_t n = src + 1; n <= dst; ++n) {
      path.vertices.push_back(n);
      ++path.cost;
    }
    return path;
  }
  // src>dst
  for (int n = static_cast<int>(src) - 1; std::cmp_greater_equal(n, dst); --n) {
    path.vertices.push_back(static_cast<size_t>(n));
    ++path.cost;
  }
  return path;
}

void calculateAllPaths() {
  paths.resize(19 * 19);
  for (const auto src :
       {0, 1, 3, 5, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}) {
    for (const auto dst :
         {0, 1, 3, 5, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}) {
      if (src == dst) {
        continue;
      }
      const auto &path =
          findPath(static_cast<size_t>(src), static_cast<size_t>(dst));
      paths[static_cast<size_t>(src + dst * 19)] = path;
    }
  }
}

// ----------------------------------------
struct Pod {
  char type;
  size_t pos;
};

static const std::array<int, 4> podMoveCost{1, 10, 100, 1000};

class Burrow {
  std::array<Pod, 8> pods;
  std::array<Pod *, 19> burrow;

private:
  void updateBurrow() {
    burrow.fill(nullptr);
    for (auto &pod : pods) {
      burrow[pod.pos] = &pod;
    }
  }

public:
  Burrow() {
    pods.fill({'.', 0});
    updateBurrow();
  }
  // copy constructor
  Burrow(const Burrow &other) {
    pods = other.pods;
    updateBurrow();
  }
  Burrow &operator=(const Burrow &other) {
    pods = other.pods;
    updateBurrow();
    return *this;
  }
  ~Burrow() = default;

  // does not check if path is valid and move allowed by rules
  // use podCanMoveTo() for that
  void movePod(size_t src, size_t dst) {
    if (burrow[src] == nullptr || burrow[dst] != nullptr) {
      return;
    }
    burrow[src]->pos = dst;
    burrow[dst] = burrow[src];
    burrow[src] = nullptr;
  }

  bool podIsHome(const Pod &pod) const {
    const size_t podHome = static_cast<size_t>((pod.type - 'A') * 2 + 11);
    if (pod.pos == podHome + 1) {
      return true;
    }
    if (pod.pos == podHome && burrow[podHome + 1] &&
        burrow[podHome + 1]->type == pod.type) {
      return true;
    }
    return false;
  }

  bool podsAreAllHome() const {
    for (const auto &pod : pods) {
      if (!podIsHome(pod)) {
        return false;
      }
    }
    return true;
  }

  bool podCanMoveTo(const Pod &pod, size_t dst) {
    // src == dst => no movement
    if (pod.pos == dst) {
      return false;
    }
    // don't stop in front of a home
    if (dst == 2 || dst == 4 || dst == 6 || dst == 8) {
      return false;
    }
    // only move to own home and don't block others
    if (dst > 10) {
      const size_t podHome = static_cast<size_t>((pod.type - 'A') * 2 + 11);
      if (dst != podHome && dst != podHome + 1) {
        return false;
      }
      if (dst == podHome && !burrow[podHome + 1]) {
        return false;
      }
      if (dst == podHome && burrow[podHome + 1] &&
          burrow[podHome + 1]->type != pod.type) {
        return false;
      }
    }
    // and stay at home
    if (podIsHome(pod)) {
      return false;
    }
    // don't move in the hallway
    if (pod.pos <= 10 && dst <= 10) {
      return false;
    }

    // destination is allowed, now check if the path is clear
    const auto checkPath = paths[pod.pos + dst * 19].vertices;
    for (auto vert = checkPath.begin() + 1; vert < checkPath.end(); ++vert) {
      if (burrow[*vert] != nullptr) {
        return false;
      }
    }

    return true;
  }

  int64_t costToHome(const int depth = 0) {
    const auto failed = std::numeric_limits<int64_t>::max() / 3;
    if (podsAreAllHome()) {
      return 0; // this was cheap
    }
    int64_t minimumPathCost = failed;
    std::vector<Path> possiblePaths{};
    for (const auto pod : pods) {
      for (const auto dst : std::views::iota(size_t{0}, size_t{19})) {
        if (podCanMoveTo(pod, dst)) {
          const auto &path = paths[pod.pos + dst * 19];
          possiblePaths.push_back(path);
          possiblePaths.back().cost *=
              podMoveCost[static_cast<size_t>(pod.type - 'A')];
        }
      }
    }

    // no paths left? backtrack
    if (possiblePaths.size() == 0) {
      return failed;
    }

    std::sort(possiblePaths.begin(), possiblePaths.end(),
              [](const auto &a, const auto &b) { return a.cost < b.cost; });

    // Do we have any pieces that can move home? Move them immediately.
    int64_t immediateCost = 0;
    for (const auto &p : possiblePaths) {
      if (p.vertices.back() <= 10) {
        continue;
      }
      movePod(p.vertices.front(), p.vertices.back());
      immediateCost += p.cost;
    }

    // DFS on all remaining paths sorted by depth
    bool foundPath = false;
    for (const auto &p : possiblePaths) {
      if (p.vertices.back() > 10) {
        continue;
      }
      // this piece was moved home
      if (burrow[p.vertices.front()] == nullptr) {
        continue;
      }
      foundPath = true;
      movePod(p.vertices.front(), p.vertices.back());
      const auto result = costToHome(depth + 1);
      if (result < failed) {
        minimumPathCost = std::min(minimumPathCost, p.cost + result);
      }
      movePod(p.vertices.back(), p.vertices.front());
    }
    // no paths remaining? Maybe the immediate moves freed up some
    // paths, or we are already home. Recurse to find out.
    if (!foundPath) {
      minimumPathCost = costToHome(depth + 1);
    }

    // Roll back immediate moves
    for (const auto &p : possiblePaths) {
      if (p.vertices.back() <= 10) {
        continue;
      }
      movePod(p.vertices.back(), p.vertices.front());
    }

    return (minimumPathCost >= failed) ? failed
                                       : minimumPathCost + immediateCost;
  }

  friend std::ifstream &operator>>(std::ifstream &in, Burrow &burrow) {
    std::string line;

    std::getline(in, line);
    std::getline(in, line);
    std::getline(in, line);
    burrow.pods[0] = {.type = line[3], .pos = 11};
    burrow.pods[1] = {.type = line[5], .pos = 13};
    burrow.pods[2] = {.type = line[7], .pos = 15};
    burrow.pods[3] = {.type = line[9], .pos = 17};

    std::getline(in, line);
    burrow.pods[4] = {.type = line[3], .pos = 12};
    burrow.pods[5] = {.type = line[5], .pos = 14};
    burrow.pods[6] = {.type = line[7], .pos = 16};
    burrow.pods[7] = {.type = line[9], .pos = 18};

    std::getline(in, line);
    burrow.updateBurrow();
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, const Burrow &burrow) {
    auto b = [burrow](const size_t pos) -> char {
      const auto bpt = burrow.burrow[pos];
      if (bpt) {
        return bpt->type;
      }
      return '.';
    };
    out << "#############\n";
    out << '#';
    for (const auto p : std::views::iota(size_t{0}, size_t{11})) {
      out << b(p);
    }
    out << "#\n";
    out << "###" << b(11) << '#' << b(13) << '#' << b(15) << '#' << b(17)
        << "###\n";
    out << "  #" << b(12) << '#' << b(14) << '#' << b(16) << '#' << b(18)
        << "#\n";
    out << "  #########\n\n";
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  calculateAllPaths();

  Burrow burrow{};

  std::ifstream infile{argv[1]};
  infile >> burrow;
  std::cout << burrow;

  const auto c = burrow.costToHome();
  std::cout << "Returning home cost " << c << " jelly beans.\n";
}
