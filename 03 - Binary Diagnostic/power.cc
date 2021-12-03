#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

class Rates {
public:
  uint32_t gamma = 0;
  uint32_t epsilon = 0;
};

class RateCounter {
private:
  std::vector<uint32_t> zero;
  std::vector<uint32_t> one;

public:
  void count(const std::string &measurement) {
    const auto len = measurement.size();
    if (zero.size() == 0) {
      zero.resize(len);
      one.resize(len);
    }
    for (size_t i = 0; i < len; ++i) {
      if (measurement[i] == '0') {
        ++zero[i];
      } else {
        ++one[i];
      }
    }
  }
  Rates gamma_epsilon() const {
    const auto len = zero.size();
    Rates r{};
    for (size_t i = 0; i < len; ++i) {
      r.gamma <<= 1;
      r.epsilon <<= 1;
      if (one[i] > zero[i]) {
        r.gamma |= 1;
      } else if (one[i] < zero[i]) {
        r.epsilon |= 1;
      }
    }
    return r;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <report.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  RateCounter counter;

  std::ifstream infile{argv[1]};
  std::string line;
  while (infile >> line) {
    counter.count(line);
  }
  const auto result = counter.gamma_epsilon();
  std::cout << "gamma = " << result.gamma << ", epsilon = " << result.epsilon
            << "\n";
  std::cout << "consumption = " << result.gamma * result.epsilon << "\n";
}
