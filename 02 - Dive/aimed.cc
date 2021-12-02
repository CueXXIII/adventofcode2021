#include <cstdlib>
#include <fstream>
#include <iostream>

struct Position {
  int horizontal = 0;
  int depth = 0;
  int aim = 0;

  Position &operator+=(Position other) {
    horizontal += other.horizontal;
    depth += other.depth;
    aim += other.aim;
    return *this;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <course.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string direction;
  int amount;
  Position subpos{};

  while (infile >> direction >> amount) {
    if (direction == "forward") {
      subpos += {amount, subpos.aim * amount, 0};
    } else if (direction == "down") {
      subpos += {0, 0, amount};
    } else if (direction == "up") {
      subpos += {0, 0, -amount};
    }
  }

  std::cout << subpos.horizontal << " * " << subpos.depth << " = "
            << subpos.horizontal * subpos.depth << "\n";
}
