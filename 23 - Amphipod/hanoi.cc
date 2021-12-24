#include "all.hpp"

struct Edge {
  const size_t v1;
  const size_t v2;
  const int cost;
};

static constexpr std::array<Edge, 18> edges{{{0, 1, 1},
                                             {1, 3, 2},
                                             {3, 5, 2},
                                             {5, 7, 2},
                                             {7, 9, 2},
                                             {9, 10, 2},
                                             {1, 11, 2},
                                             {3, 11, 2},
                                             {11, 12, 1},
                                             {3, 13, 2},
                                             {5, 13, 2},
                                             {13, 14, 1},
                                             {5, 15, 2},
                                             {7, 15, 2},
                                             {15, 16, 1},
                                             {7, 17, 2},
                                             {9, 17, 2},
                                             {17, 18, 1}}};

struct Pod {
  char type;
  size_t pos;
};

class Burrow {
  std::array<Pod, 8> pods;

public:
  Burrow() { pods.fill({'.',0});}

  friend std::ifstream &operator>>(std::ifstream &in, Burrow &burrow) {
    std::string line;

    std::getline(in, line);
    std::getline(in, line);
    std::getline(in, line);
    burrow.pods[0] = {line[3], 11};
    burrow.pods[1] = {line[5], 13};
    burrow.pods[2] = {line[7], 15};
    burrow.pods[3] = {line[9], 17};

    std::getline(in, line);
    burrow.pods[4] = {line[3], 12};
    burrow.pods[5] = {line[5], 14};
    burrow.pods[6] = {line[7], 16};
    burrow.pods[7] = {line[9], 18};

    std::getline(in, line);
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, const Burrow &burrow) {
    std::array<char, 19> b;
    b.fill('.');
    for (const auto &pod : burrow.pods) {
      b[pod.pos] = pod.type;
    }
    out << "#############\n";
    out << '#';
    for (const auto p : std::views::iota(size_t{0}, size_t{11})) {
      out << b[p];
    }
    out << "#\n";
    out << "###" << b[11] << '#' << b[13] << '#' << b[15] << '#' << b[17]
        << "###\n";
    out << "  #" << b[12] << '#' << b[14] << '#' << b[16] << '#' << b[18]
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

  Burrow burrow{};
  std::cout << burrow;
  std::ifstream infile{argv[1]};
  infile >> burrow;
  std::cout << burrow;
}
