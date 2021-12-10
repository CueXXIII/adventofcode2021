#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>

enum Direction { open, close };

class Bracket {
public:
  uint8_t type;
  enum Direction dir;
};

Bracket parseBracket(char c) {
  switch (c) {
  case '(':
    return {0, open};
  case ')':
    return {0, close};
  case '[':
    return {1, open};
  case ']':
    return {1, close};
  case '{':
    return {2, open};
  case '}':
    return {2, close};
  case '<':
    return {3, open};
  case '>':
    return {3, close};
  }
  // not reached, don't care
  return {0, open};
}

class Line {
private:
  std::string source{};

  bool isCorrupt{false};
  char corruptToken{' '};

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
    return in;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <subsystem.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  uint64_t corruptScore{0};
  for (;;) {
    Line line;
    if (!(infile >> line)) {
      break;
    }
    corruptScore += line.corruptScore();
  }
  std::cout << "This syntax scores " << corruptScore << " points for FIC.\n";
}
