#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

class Display {
private:
  std::array<std::string, 10> digits{};
  std::array<std::string, 4> value{};
  int simpleDigits{0};

public:
  int getSimpleDigits() const { return simpleDigits; }

  friend std::ifstream &operator>>(std::ifstream &in, Display &d) {
    std::string token;
    for (auto i = 0; i < 10; ++i) {
      if (!(in >> token)) {
        return in;
      }
      d.digits[i] = token;
    }
    if (!(in >> token)) {
      return in;
    }
    if (token != "|") {
      in.close();
      return in;
    }
    for (auto i = 0; i < 4; ++i) {
      if (!(in >> token)) {
        return in;
      }
      d.value[i] = token;
      switch (token.length()) {
      case 2: // digit 1
      case 3: // digit 7
      case 4: // digit 4
      case 7: // digit 8
        ++d.simpleDigits;
        break;
      }
    }
    return in;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <segments.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::vector<Display> displays;

  std::ifstream infile{argv[1]};
  for (;;) {
    Display currentDisplay{};
    if (!(infile >> currentDisplay)) {
      break;
    }
    displays.push_back(currentDisplay);
  }
  const auto simpleDigits{
      std::accumulate(displays.begin(), displays.end(), 0,
                      [](auto a, auto b) { return a + b.getSimpleDigits(); })};
  std::cout << "The digits 1, 4, 7 and 8 are displayed " << simpleDigits
            << ".\n";
}
