#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>

struct SnailDigit {
  int depth;
  int value;
};

class SnailNumber {
  std::list<SnailDigit> digits;

public:
  SnailNumber(std::string input) {
    int currentDepth = 0;
    for (const char c : input) {
      if (c == '[') {
        ++currentDepth;
      } else if (c == ']') {
        --currentDepth;
      } else if (c >= '0' && c <= '9') {
        digits.emplace_back(currentDepth, c - '0');
      }
    }
  }

  void reduce() {
    bool action = true;
    while (action) {
      action = false;
      // explode?
      for (auto pos = digits.begin(); pos != digits.end(); ++pos) {
        if (pos->depth == 5) {
          action = true;
          auto prev = pos;
          --prev;
          auto pair = pos;
          ++pair;
          auto next = pair;
          ++next;
          if (pos != digits.begin()) {
            prev->value += pos->value;
          }
          if (next != digits.end()) {
            next->value += pair->value;
          }
          *pos = {pos->depth - 1, 0};
          digits.erase(pair);
          break;
        }
      }
      if (action) {
        continue;
      }
      // split?
      for (auto pos = digits.begin(); pos != digits.end(); ++pos) {
        if (pos->value > 9) {
          action = true;
          const auto lefthalf = pos->value / 2;
          const auto righthalf = (pos->value + 1) / 2;
          pos->value = righthalf;
          pos->depth += 1;
          digits.emplace(pos, pos->depth, lefthalf);
          break;
        }
      }
    }
  }

  void add(SnailNumber &other) {
    digits.splice(digits.end(), other.digits);
    for (auto &digit : digits) {
      digit.depth += 1;
    }
    reduce();
  }

private:
  auto calcMagnitude(auto position, int depth = 1) const {
    if (position == digits.end()) {
      return std::pair(position, 0);
    }
    int sum = 0;
    // left
    if (depth < position->depth) {
      auto [newPos, leftSum] = calcMagnitude(position, depth + 1);
      sum += 3 * leftSum;
      position = newPos;
    } else {
      sum += 3 * position->value;
      ++position;
    }
    // right
    if (depth < position->depth) {
      auto [newPos, rightSum] = calcMagnitude(position, depth + 1);
      sum += 2 * rightSum;
      position = newPos;
    } else {
      sum += 2 * position->value;
      ++position;
    }
    return std::pair(position, sum);
  }

public:
  auto magnitude() const { return calcMagnitude(digits.begin()).second; }

private:
  auto print(std::ostream &out, auto position, int depth = 1) {
    if (position == digits.end()) {
      return position;
    }
    // left
    out << '[';
    if (depth < position->depth) {
      position = print(out, position, depth + 1);
    } else {
      out << position->value;
      ++position;
    }
    out << ',';
    // right
    if (depth < position->depth) {
      position = print(out, position, depth + 1);
    } else {
      out << position->value;
      ++position;
    }
    out << ']';
    return position;
  }

public:
  friend std::ostream &operator<<(std::ostream &out, SnailNumber n) {
    n.print(out, n.digits.begin());
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <homework.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;
  infile >> line;
  SnailNumber sum{line};
  while (infile >> line) {
    SnailNumber second{line};
    std::cout << "  " << sum << "\n+ " << second << "\n= ";
    sum.add(second);
    std::cout << sum << " magnitude " << sum.magnitude() << "\n\n";
  }
}
