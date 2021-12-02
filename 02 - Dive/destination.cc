#include <cstdlib>
#include <fstream>
#include <iostream>

struct Position {
  int x = 0;
  int y = 0;

  Position &operator+=(Position other) {
    x += other.x;
    y += other.y;
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
      subpos += {amount, 0};
    } else if (direction == "down") {
      subpos += {0, amount};
    } else if (direction == "up") {
      subpos += {0, -amount};
    }
  }

  std::cout << subpos.x << " * " << subpos.y << " = " << subpos.x * subpos.y
            << "\n";
}
