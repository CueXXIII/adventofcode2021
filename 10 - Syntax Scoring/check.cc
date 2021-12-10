#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "median.hpp"

enum Direction { open, close };

class Bracket {
public:
  uint8_t type;
  enum Direction dir;
};

Bracket parseBracket(char c) {
  switch (c) {
  case '(':
    return {1, open};
  case ')':
    return {1, close};
  case '[':
    return {2, open};
  case ']':
    return {2, close};
  case '{':
    return {3, open};
  case '}':
    return {3, close};
  case '<':
    return {4, open};
  case '>':
    return {4, close};
  }
  // not reached, don't care
  return {0, open};
}

class Line {
private:
  std::string source{};

  bool isCorrupt{false};
  char corruptToken{' '};

  uint64_t autocomplete{0};

public:
  uint64_t corruptScore() const {
    if (isCorrupt) {
      switch (corruptToken) {
      case ')':
        return 3;
      case ']':
        return 57;
      case '}':
        return 1197;
      case '>':
        return 25137;
      }
    }
    return 0;
  }

  uint64_t autocompleteScore() const { return autocomplete; }

  friend std::ifstream &operator>>(std::ifstream &in, Line &line) {
    if (!(in >> line.source)) {
      return in;
    }
    std::stack<Bracket> stack{};
    for (const auto c : line.source) {
      const Bracket token{parseBracket(c)};
      if (token.dir == open) {
        stack.push(token);
      } else {
        if (stack.top().type == token.type) {
          stack.pop();
        } else {
          line.isCorrupt = true;
          line.corruptToken = c;
          break;
        }
      }
    }
    if (!line.isCorrupt) {
      while (!stack.empty()) {
        line.autocomplete *= 5;
        line.autocomplete += stack.top().type;
        stack.pop();
      }
    }
    return in;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <subsystem.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  uint64_t FIC{0};
  std::vector<uint64_t> ACS{};
  for (;;) {
    Line line;
    if (!(infile >> line)) {
      break;
    }
    FIC += line.corruptScore();
    const uint64_t lineACS = line.autocompleteScore();
    if (lineACS > 0) {
      ACS.push_back(lineACS);
    }
  }
  const auto ACSmedian = aoc::median(ACS);

  std::cout << "This syntax scores " << FIC << " points for FIC.\n";
  std::cout << "The overall ACS is " << ACSmedian << " points.\n";
}
