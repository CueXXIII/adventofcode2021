#include "all.hpp"

const std::vector<std::pair<int, int>> universeSplits = {
    {3, 1}, {4, 3}, {5, 6}, {6, 7}, {7, 6}, {8, 3}, {9, 1}};

class PlayerScores {
  std::vector<int64_t> multiverses;

public:
  PlayerScores() { multiverses.resize(10 * 10 * 21 * 21, 0); }
  int64_t &at(int p1, int p2, int s1, int s2) {
    return multiverses[static_cast<size_t>((p1 - 1) + (p2 - 1) * 10 +
                                           s1 * 10 * 10 + s2 * 10 * 10 * 21)];
  }
  int64_t count() const {
    return std::accumulate(begin(multiverses), end(multiverses), int64_t{0});
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <board.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;

  std::getline(infile, line);
  const int player1Start = line[line.size() - 1] - '0';
  std::getline(infile, line);
  const int player2Start = line[line.size() - 1] - '0';

  PlayerScores boards{};
  // initial universe
  boards.at(player1Start, player2Start, 0, 0) = 1;
  int64_t p1Win = 0;
  int64_t p2Win = 0;

  while (boards.count() > 0) {
    // player 1 //
    // iterate backwards to be able to do updates inplace
    for (int s1 = 20; s1 >= 0; --s1) {
      for (int s2 = 20; s2 >= 0; --s2) {
        for (int p1 = 10; p1 >= 1; --p1) {
          for (int p2 = 10; p2 >= 1; --p2) {
            const int64_t universes = boards.at(p1, p2, s1, s2);
            if (universes > 0) {
              boards.at(p1, p2, s1, s2) = 0;
              for (const auto &rt : universeSplits) {
                const auto roll = rt.first;
                const int64_t times = rt.second;
                const int newPos = (p1 + roll - 1) % 10 + 1;
                const int newScore = s1 + newPos;
                if (newScore < 21) {
                  boards.at(newPos, p2, newScore, s2) += universes * times;
                } else {
                  p1Win += universes * times;
                }
              }
            }
          }
        }
      }
    }

    // player 2 //
    // iterate backwards to be able to do updates inplace
    for (int s1 = 20; s1 >= 0; --s1) {
      for (int s2 = 20; s2 >= 0; --s2) {
        for (int p1 = 10; p1 >= 1; --p1) {
          for (int p2 = 10; p2 >= 1; --p2) {
            const int64_t universes = boards.at(p1, p2, s1, s2);
            if (universes > 0) {
              boards.at(p1, p2, s1, s2) = 0;
              for (const auto &rt : universeSplits) {
                const auto roll = rt.first;
                const int64_t times = rt.second;
                const int newPos = (p2 + roll - 1) % 10 + 1;
                const int newScore = s2 + newPos;
                if (newScore < 21) {
                  boards.at(p1, newPos, s1, newScore) += universes * times;
                } else {
                  p2Win += universes * times;
                }
              }
            }
          }
        }
      }
    }
  }
  std::cout << fmt::format("{:d} Player 1 wins\n", p1Win);
  std::cout << fmt::format("{:d} Player 2 wins\n", p2Win);
  // and The Answer™ is:
  std::cout << fmt::format("42 = {:d}\n", std::max(p1Win, p2Win));
}
