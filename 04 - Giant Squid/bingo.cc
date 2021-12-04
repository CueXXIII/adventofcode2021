#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

class bingoBoard {
private:
  std::array<uint8_t, 25> board{};
  std::array<bool, 25> drawn{false};
  uint8_t winningNumber = 255;

  bool isMarked(size_t position) const { return drawn[position]; }

  void mark(size_t position) { drawn[position] = true; }

  bool hasBingo() const {
    for (auto n = 0; n < 5; ++n) {
      bool isSet = true;
      for (auto row = 0; row < 5; ++row) {
        if (!isMarked(row + 5 * n)) {
          isSet = false;
        }
      }
      if (isSet) {
        return true;
      }

      isSet = true;
      for (auto col = 0; col < 5; ++col) {
        if (!isMarked(col * 5 + n)) {
          isSet = false;
        }
      }
      if (isSet) {
        return true;
      }
    }
    return false;
  }

public:
  // rule of zero: use default destruct, copy, assign

  bool ballDrawn(uint8_t ball) {
    if (winningNumber == 255) {
      for (auto i = 0; i < 25; ++i) {
        if (board[i] == ball) {
          mark(i);
        }
      }
      if (hasBingo()) {
        winningNumber = ball;
        return true;
      }
    }
    return false;
  }

  bool isWinner() const { return winningNumber != 255; }

  uint32_t winningCode() const {
    uint32_t unmarkedSum = 0;
    for (auto n = 0; n < 25; ++n) {
      if (!isMarked(n)) {
        unmarkedSum += board[n];
      }
    }
    return unmarkedSum * winningNumber;
  }

  friend std::ifstream &operator>>(std::ifstream &infile, bingoBoard &board) {
    if (!(infile >> reinterpret_cast<int &>(board.board[0]))) {
      return infile;
    }

    for (int position = 1; position < 25; ++position) {
      if (!(infile >> reinterpret_cast<int &>(board.board[position]))) {
        std::cerr << "premature end of input\n";
        std::exit(EXIT_FAILURE);
      }
    }
    return infile;
  }

  friend std::ostream &operator<<(std::ostream &out, const bingoBoard &board) {
    for (auto x = 0; x < 5; ++x) {
      for (auto pos = x; pos < 25; pos += 5) {
        if (pos > 4) {
          std::cout << " ";
        }
        if (board.board[pos] < 10) {
          std::cout << " ";
        }
        if (board.isMarked(pos)) {
          std::cout << "*";
        } else {
          std::cout << " ";
        }
        std::cout << static_cast<int>(board.board[pos]);
      }
      std::cout << "\n";
    }
    if (board.winningNumber != 255) {
      std::cout << "Winning with number "
                << static_cast<int>(board.winningNumber) << "\n";
    }
    std::cout << "\n";
    return out;
  }
};

class Draws {
private:
  std::vector<uint8_t> numbers;

public:
  auto begin() const { return numbers.begin(); }

  auto end() const { return numbers.end(); }

  friend std::ifstream &operator>>(std::ifstream &infile, Draws &draws) {
    std::string line;
    infile >> line;
    const char *cline = line.c_str();
    char *cend;
    while (cline[0] != '\0') {
      const auto ball = strtoul(cline, &cend, 10);
      draws.numbers.push_back(static_cast<uint8_t>(ball));
      if (cend[0] == ',') {
        ++cend;
      }
      cline = cend;
    }
    return infile;
  }

  friend std::ostream &operator<<(std::ostream &out, const Draws &draws) {
    for (const auto ball : draws.numbers) {
      out << static_cast<int>(ball) << " ";
    }
    out << "\n";
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <bingo.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};

  Draws draws;
  infile >> draws;

  std::vector<bingoBoard> boards;
  while (true) {
    bingoBoard board;
    if (!(infile >> board)) {
      break;
    }
    boards.push_back(board);
  }

  for (const auto ball : draws) {
    std::cout << "Drawing " << static_cast<int>(ball) << "\n";
    for (auto &board : boards) {
      auto res = board.ballDrawn(ball);
      if (res) {
        std::cout << board;
        std::cout << "We have a winner! Code=" << board.winningCode() << "\n";
	return EXIT_SUCCESS;
      }
    }
  }
}
