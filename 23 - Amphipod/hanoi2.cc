#include "all.hpp"

// Burrow positions
// 0 -- 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 -- 8 -- 9 -- 10
//           |         |         |         |
//          11        15        19        23
//           |         |         |         |
//          12        16        20        24
//           |         |         |         |
//          13        17        21        25
//           |         |         |         |
//          14        18        22        26

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
  path.cost = -1;
  if (src == dst) {
    path.vertices.push_back(src);
    ++path.cost;
    return path;
  }
  if (src > 10 && dst > 10 && ((src + 1) | 3) == ((dst + 1) | 3)) {
    // this path is invalid
    path.vertices = {src, dst};
    path.cost = 99;
    return path;
  }
  if (src > 10) {
    while ((src & 3) != 3) {
      path.vertices.push_back(src);
      ++path.cost;
      --src;
    }
    path.vertices.push_back(src);
    ++path.cost;
    src = src / 2 - 3;
    Path section = findPath(src, dst);
    for (const auto v : section.vertices) {
      path.vertices.push_back(v);
    }
    path.cost += section.cost + 1;
    return path;
  }
  if (dst > 10) {
    const size_t wayp = ((dst + 1) | 3) / 2 - 5;
    path = findPath(src, wayp);
    src = wayp * 2 + 7;
    while (src <= dst) {
      path.vertices.push_back(src);
      ++path.cost;
      ++src;
    }
    return path;
  }
  if (src < dst) {
    for (size_t n = src; n <= dst; ++n) {
      path.vertices.push_back(n);
      ++path.cost;
    }
    return path;
  }
  // src>dst
  for (size_t n = src + 1; n-- > dst;) {
    path.vertices.push_back(n);
    ++path.cost;
  }
  return path;
}

void calculateAllPaths() {
  paths.resize(27 * 27);
  for (const auto src : {0,  1,  3,  5,  7,  9,  10, 11, 12, 13, 14, 15,
                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}) {
    for (const auto dst : {0,  1,  3,  5,  7,  9,  10, 11, 12, 13, 14, 15,
                           16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}) {
      if (src == dst) {
        continue;
      }
      const auto &path =
          findPath(static_cast<size_t>(src), static_cast<size_t>(dst));
      paths[static_cast<size_t>(src + dst * 27)] = path;
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
  std::array<Pod, 16> pods;
  std::array<Pod *, 27> burrow;

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
  Burrow(const Burrow &) = delete;
  Burrow &operator=(const Burrow &) = delete;
  Burrow(Burrow &&) = delete;
  Burrow &operator=(Burrow &&) = delete;
  ~Burrow() = default;

  // does not check if path is valid and move allowed by rules
  // use podCanMoveTo() for that
  void movePod(size_t src, size_t dst) {
    if (burrow[src] == nullptr || burrow[dst] != nullptr) {
      std::cout << "Illegal move! " << src << " -> " << dst << "\n";
      return;
    }
    burrow[src]->pos = dst;
    burrow[dst] = burrow[src];
    burrow[src] = nullptr;
  }

  bool podIsHome(const Pod &pod) const {
    const size_t podBase = static_cast<size_t>((pod.type - 'A') * 4 + 14);
    for (const auto d : std::views::iota(size_t{0}, size_t{4})) {
      if (pod.pos == podBase - d) {
        return true;
      }
      if (burrow[podBase - d] == nullptr ||
          burrow[podBase - d]->type != pod.type) {
        return false;
      }
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
      const size_t podBase = static_cast<size_t>((pod.type - 'A') * 4 + 14);
      if (dst < podBase - 3 || dst > podBase) {
        return false;
      }
      for (const auto pos : std::views::iota(dst + 1, podBase + 1)) {
        if (burrow[pos] == nullptr || burrow[pos]->type != pod.type) {
          return false;
        }
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
    const auto checkPath = paths[pod.pos + dst * 27].vertices;
    for (auto vert = checkPath.begin() + 1; vert < checkPath.end(); ++vert) {
      if (burrow[*vert] != nullptr) {
        return false;
      }
    }

    return true;
  }

  void printSolution(const std::vector<Path> &solution) const {
    int64_t totalCost = 0;
    std::cout << "Possible solution:\n";
    for (const auto &p : solution) {
      std::cout << "  (" << p.vertices.front() << ")->(" << p.vertices.back()
                << ") for " << p.cost << "\n";
      totalCost += p.cost;
    }
    std::cout << "  totalCost = " << totalCost << " jelly beans\n\n";
  }

  int64_t costToHomeMin;
  void costToHome(
      const int depth = 0, const int64_t currentCost = 0,
      std::vector<Path> &solution = *std::make_shared<std::vector<Path>>()) {
    const auto infinity = std::numeric_limits<int64_t>::max() / 3;
    if (depth == 0) {
      costToHomeMin = infinity;
    }
    if (currentCost > costToHomeMin) {
      return;
    }
    if (podsAreAllHome()) {
      if (currentCost < costToHomeMin) {
        costToHomeMin = currentCost;
        printSolution(solution);
      }
      return; // this was cheap
    }
    std::vector<Path> possiblePaths{};
    for (const auto pod : pods) {
      for (const auto dstRev : std::views::iota(size_t{0}, size_t{27})) {
        const auto dst = 26 - dstRev;
        const auto &path = paths[pod.pos + dst * 27];
        if (path.cost < 99 && podCanMoveTo(pod, dst)) {
          possiblePaths.push_back(path);
          possiblePaths.back().cost *=
              podMoveCost[static_cast<size_t>(pod.type - 'A')];
          // don't add more paths if this pod can move home
          if (dst > 10) {
            break;
          }
        }
      }
    }

    std::sort(possiblePaths.begin(), possiblePaths.end(),
              [](const auto &a, const auto &b) { return a.cost < b.cost; });

    // DFS on all remaining paths sorted by depth
    for (const auto &p : possiblePaths) {
      movePod(p.vertices.front(), p.vertices.back());
      solution.push_back(p);
      costToHome(depth + 1, currentCost + p.cost, solution);
      movePod(p.vertices.back(), p.vertices.front());
      solution.pop_back();
    }
  }

  friend std::ifstream &operator>>(std::ifstream &in, Burrow &burrow) {
    std::string line;

    std::getline(in, line);
    std::getline(in, line);
    std::getline(in, line);
    burrow.pods[0] = {.type = line[3], .pos = 11};
    burrow.pods[1] = {.type = line[5], .pos = 15};
    burrow.pods[2] = {.type = line[7], .pos = 19};
    burrow.pods[3] = {.type = line[9], .pos = 23};

    std::getline(in, line);
    burrow.pods[4] = {.type = line[3], .pos = 14};
    burrow.pods[5] = {.type = line[5], .pos = 18};
    burrow.pods[6] = {.type = line[7], .pos = 22};
    burrow.pods[7] = {.type = line[9], .pos = 26};

    std::getline(in, line);

    // folded positions
    burrow.pods[8] = {.type = 'D', .pos = 12};
    burrow.pods[9] = {.type = 'C', .pos = 16};
    burrow.pods[10] = {.type = 'B', .pos = 20};
    burrow.pods[11] = {.type = 'A', .pos = 24};

    burrow.pods[12] = {.type = 'D', .pos = 13};
    burrow.pods[13] = {.type = 'B', .pos = 17};
    burrow.pods[14] = {.type = 'A', .pos = 21};
    burrow.pods[15] = {.type = 'C', .pos = 25};

    burrow.updateBurrow();
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, const Burrow &burrow) {
    auto b = [&burrow](const size_t pos) -> char {
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
    out << "###" << b(11) << '#' << b(15) << '#' << b(19) << '#' << b(23)
        << "###\n";
    out << "  #" << b(12) << '#' << b(16) << '#' << b(20) << '#' << b(24)
        << "#\n";
    out << "  #" << b(13) << '#' << b(17) << '#' << b(21) << '#' << b(25)
        << "#\n";
    out << "  #" << b(14) << '#' << b(18) << '#' << b(22) << '#' << b(26)
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

  burrow.costToHome();
  std::cout << "\nReturning home cost " << burrow.costToHomeMin
            << " jelly beans.\n";
}
