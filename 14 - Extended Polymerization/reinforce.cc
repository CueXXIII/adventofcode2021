#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

class Rule {
private:
  const uint16_t input; // needed?
  const std::array<const uint16_t, 2> output;

public:
  Rule(const std::string &rule)
      : input{static_cast<uint16_t>((rule[0] << 8) | (rule[1]))},
        output{static_cast<uint16_t>((rule[0] << 8) | (rule[6])),
               static_cast<uint16_t>((rule[6] << 8) | (rule[1]))} {}

  decltype(output) get() const { return output; }
};

class Template {
  std::unordered_map<uint16_t, uint64_t> pairs;
  char lastElement;
  std::unordered_map<uint16_t, Rule> rules;

public:
  Template(const std::string &polymer) {
    for (auto pos = size_t{0}; pos < polymer.size() - 1; ++pos) {
      const uint16_t elementPair{
          static_cast<uint16_t>((polymer[pos] << 8) | (polymer[pos + 1]))};
      ++pairs[elementPair];
    }
    lastElement = polymer[polymer.size() - 1];
  }

  void addRule(const std::string &rule) {
    const uint16_t id{static_cast<uint16_t>((rule[0] << 8) | (rule[1]))};
    rules.emplace(id, rule);
  }

  void step() {
    decltype(pairs) nextPairs;
    for (const auto &pair : pairs) {
      const auto &match = rules.find(pair.first);
      if (match != rules.end()) {
        const auto &insertion{match->second.get()};
        nextPairs[insertion[0]] += pair.second;
        nextPairs[insertion[1]] += pair.second;
      } else {
        std::cout << "Not found pair " << pair.first << "\n";
        nextPairs[pair.first] += pair.second;
      }
    }
    pairs = std::move(nextPairs);
  }

  friend std::ostream &operator<<(std::ostream &out, Template &t) {
    std::array<uint64_t, 128> count;
    count.fill(0);
    for (const auto &pair : t.pairs) {
      count[pair.first >> 8] += pair.second;
    }
    count[t.lastElement]++;

    out << "{";
    bool first = true;
    uint64_t min{0}, max{0};
    for (auto element = 'A'; element <= 'Z'; ++element) {
      if (count[element] > 0) {
        if (!first) {
          out << ", ";
        } else {
          min = max = count[element];
          first = false;
        }
        out << element << ":" << count[element];
        min = std::min(min, count[element]);
        max = std::max(max, count[element]);
      }
    }
    out << "} q=" << max - min;
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <rules.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;
  std::getline(infile, line);
  Template t{line};
  std::getline(infile, line);
  while (std::getline(infile, line)) {
    t.addRule(line);
  }

  std::cout << t << "\n";
  for (auto i = size_t{0}; i < 10; ++i) {
    t.step();
  }
  std::cout << t << "\n";
}
