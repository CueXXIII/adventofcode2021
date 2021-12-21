#include "all.hpp"

class Dice {
  int top;
  int totalRolls = 0;

public:
  Dice(int top) : top(top) {}
  int roll() {
    const int ret = top;
    top = (top % 100) + 1;
    ++totalRolls;
    return ret;
  }
  int rollCount() const { return totalRolls; }
};

class Player {
  const int id;
  int pos;
  int score = 0;

public:
  Player(const int id, const int pos) : id(id), pos(pos) {}
  void roll(Dice &die) {
    const int r1 = die.roll();
    const int r2 = die.roll();
    const int r3 = die.roll();
    const int total = r1 + r2 + r3;
    pos += total;
    pos = (pos - 1) % 10 + 1;
    score += pos;
    std::cout << fmt::format("Player {:d} rolls {:d}+{:d}+{:d} and moves to "
                             "space {:d} for a total score of {:d}.\n",
                             id, r1, r2, r3, pos, score);
  }
  bool hasWon() const { return score >= 1000; }
  int getScore() const { return score; }
};

class Players {
  std::vector<Player> players;
  size_t turnPlayer = 1;

public:
  void newPlayer(int pos) { players.emplace_back(players.size() + 1, pos); }
  Player &onTurn() { return players[turnPlayer - 1]; }
  Player &waiting() { return players[(3 - turnPlayer) - 1]; }
  void nextPlayer() { turnPlayer = 3 - turnPlayer; }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <board.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;

  Players players{};
  std::getline(infile, line);
  players.newPlayer(line[line.size() - 1] - '0');
  std::getline(infile, line);
  players.newPlayer(line[line.size() - 1] - '0');

  Dice die{1};

  while (true) {
    players.onTurn().roll(die);
    if (players.onTurn().hasWon()) {
      const int highscore = players.waiting().getScore() * die.rollCount();
      std::cout << fmt::format("This game scored {:d} little things.\n",
                               highscore);
      break;
    }
    players.nextPlayer();
  }
}
