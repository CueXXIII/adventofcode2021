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
  std::array<uint8_t, 7> segmentWeights{};

  // see deduction.txt
  static int weightToInt(const int weight) {
    switch (weight) {
    case 17:
      return 1;
    case 25:
      return 7;
    case 30:
      return 4;
    case 34:
      return 2;
    case 37:
      return 5;
    case 39:
      return 3;
    case 41:
      return 6;
    case 42:
      return 0;
    case 45:
      return 9;
    case 49:
      return 8;
    }
    // only reached for malformed input -> don't care
    return -1;
  };

  int getSegmentWeight(const char segment) const {
    return segmentWeights[segment - 'a'];
  }

  int getDigitWeight(const std::string &segments) const {
    return std::accumulate(
        segments.begin(), segments.end(), 0,
        [this](auto a, auto b) { return a + this->getSegmentWeight(b); });
  }

public:
  int getSimpleDigits() const { return simpleDigits; }

  int getValue() const {
    return std::accumulate(
        value.begin(), value.end(), 0, [this](auto a, auto b) {
          return a * 10 + Display::weightToInt(this->getDigitWeight(b));
        });
  }

  friend std::ifstream &operator>>(std::ifstream &in, Display &d) {
    std::string token;
    for (auto i = 0; i < 10; ++i) {
      if (!(in >> token)) {
        return in;
      }
      d.digits[i] = token;
      for (const auto c : token) {
        ++d.segmentWeights[c - 'a'];
      }
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

  friend std::ostream &operator<<(std::ostream &out, Display &d) {
    out << "Displayed segments:";
    for (const auto &s : d.value) {
      out << " " << s;
    }
    out << "\nSimple digits (1,4,7,8): " << d.simpleDigits;
    out << "\nSegment weights: ";
    for (const auto w : d.segmentWeights) {
      out << static_cast<char>('0' + w);
    }
    out << "\nDisplayed number: " << d.getValue();
    return out << "\n\n";
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

  const auto valueSums{
      std::accumulate(displays.begin(), displays.end(), 0,
                      [](auto a, auto b) { return a + b.getValue(); })};

  std::cout << "The sum of all values displayed is " << valueSums << ".\n";
}
