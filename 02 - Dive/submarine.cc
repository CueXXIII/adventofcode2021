#include <cstdlib>
#include <fstream>
#include <iostream>

class Submarine {
protected:
  int64_t horiz = 0;
  int64_t depth = 0;

public:
  virtual void move(std::string direction, int64_t distance) {
    if (direction == "forward") {
      horiz += distance;
    } else if (direction == "down") {
      depth += distance;
    } else if (direction == "up") {
      depth -= distance;
    }
  }

  int64_t encodePosition() const { return horiz * depth; }
};

class AimedSubmarine : public Submarine {
protected:
  int64_t aim = 0;

public:
  virtual void move(std::string direction, int64_t distance) override {
    if (direction == "forward") {
      horiz += distance;
      depth += aim * distance;
    } else if (direction == "down") {
      aim += distance;
    } else if (direction == "up") {
      aim -= distance;
    }
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <course.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  Submarine sub1{};
  AimedSubmarine sub2{};

  std::ifstream infile{argv[1]};

  std::string token;
  int64_t number;
  while (infile >> token >> number) {
    sub1.move(token, number);
    sub2.move(token, number);
  }

  std::cout << "Submarine acc. manual = " << sub1.encodePosition() << "\n";
  std::cout << "Submarine in real sea = " << sub2.encodePosition() << "\n";
}
