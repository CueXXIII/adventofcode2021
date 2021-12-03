#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

class PowerRates {
public:
  uint32_t gamma = 0;
  uint32_t epsilon = 0;
};

class PowerRateCounter {
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
  PowerRates gamma_epsilon() const {
    const auto len = zero.size();
    PowerRates r{};
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

class LifeSupportRates {
public:
  uint32_t o2 = 0;
  uint32_t co2 = 0;
};

class LifeSupportCounter {
private:
  size_t bits;
  std::vector<uint32_t> o2Values{};
  std::vector<uint32_t> co2Values{};

  void countO2() {
    for (size_t currentMask = 1 << (bits - 1);
         currentMask > 0 && o2Values.size() > 1; currentMask >>= 1) {
      uint32_t zeroCount = 0;
      uint32_t oneCount = 0;
      for (const auto value : o2Values) {
        if ((value & currentMask) == 0) {
          ++zeroCount;
        } else {
          ++oneCount;
        }
      }
      std::vector<uint32_t> newValues{};
      for (const auto value : o2Values) {
        if (oneCount >= zeroCount) {
          if ((value & currentMask) != 0) {
            newValues.push_back(value);
          }
        } else {
          if ((value & currentMask) == 0) {
            newValues.push_back(value);
          }
        }
      }
      o2Values = std::move(newValues);
    }
  }

  void countCO2() {
    for (size_t currentMask = 1 << (bits - 1);
         currentMask > 0 && co2Values.size() > 1; currentMask >>= 1) {
      uint32_t zeroCount = 0;
      uint32_t oneCount = 0;
      for (const auto value : co2Values) {
        if ((value & currentMask) == 0) {
          ++zeroCount;
        } else {
          ++oneCount;
        }
      }
      std::vector<uint32_t> newValues{};
      for (const auto value : co2Values) {
        if (zeroCount > oneCount) {
          if ((value & currentMask) != 0) {
            newValues.push_back(value);
          }
        } else {
          if ((value & currentMask) == 0) {
            newValues.push_back(value);
          }
        }
      }
      co2Values = std::move(newValues);
    }
  }

public:
  void count(std::string &measurement) {
    bits = measurement.size();
    const uint32_t value =
        static_cast<uint32_t>(std::stoul(measurement, nullptr, 2));
    o2Values.push_back(value);
    co2Values.push_back(value);
  }

  LifeSupportRates o2co2() {
    countO2();
    countCO2();
    return {.o2 = o2Values[0], .co2 = co2Values[0]};
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <report.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  PowerRateCounter power;
  LifeSupportCounter life;

  std::ifstream infile{argv[1]};
  std::string line;
  while (infile >> line) {
    power.count(line);
    life.count(line);
  }
  const auto consumption = power.gamma_epsilon();
  std::cout << "gamma = " << consumption.gamma
            << ", epsilon = " << consumption.epsilon << "\n";
  std::cout << "consumption = " << consumption.gamma * consumption.epsilon
            << "\n";
  const auto lsRating = life.o2co2();
  std::cout << "oxygen = " << lsRating.o2 << ", co2 scrubber = " << lsRating.co2
            << "\n";
  std::cout << "life support diagnostics = " << lsRating.o2 * lsRating.co2
            << "\n";
}
