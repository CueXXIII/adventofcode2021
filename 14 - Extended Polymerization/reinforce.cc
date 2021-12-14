#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>

using charPair = std::array<char, 2>;

template <> struct std::hash<charPair> {
  std::size_t operator()(const charPair &word) const noexcept {
    return std::hash<int>{}(word[0] << 8 | word[1]);
  }
};

std::ostream &operator<<(std::ostream &out, const charPair &c) {
  return out << c[0] << c[1];
};

class Rule {
private:
  const std::array<const charPair, 2> output;

public:
  Rule(const std::string &rule)
      : output{charPair{rule[0], rule[6]}, charPair{rule[6], rule[1]}} {}

  decltype(output) get() const { return output; }
};

class Template {
  std::unordered_map<charPair, uint64_t> pairs;
  char lastElement;
  std::unordered_map<charPair, Rule> rules;

public:
  Template(const std::string &polymer) {
    for (auto pos = size_t{0}; pos < polymer.size() - 1; ++pos) {
      const charPair elementPair{polymer[pos], polymer[pos + 1]};
      ++pairs[elementPair];
    }
    lastElement = polymer[polymer.size() - 1];
  }

  void addRule(const std::string &rule) {
    const charPair id{rule[0], rule[1]};
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
    std::map<char, uint64_t> count;
    for (const auto &pair : t.pairs) {
      count[pair.first[0]] += pair.second;
    }
    count[t.lastElement]++;

    out << "ElementCount = {";
    bool first = true;
    uint64_t min{0}, max{0};
    for (const auto &element : count) {
      if (element.second > 0) {
        if (!first) {
          out << ", ";
        } else {
          min = max = element.second;
          first = false;
        }
        out << element.first << ":" << element.second;
        min = std::min(min, element.second);
        max = std::max(max, element.second);
      }
    }
    out << "}\nQuantitiy value = " << max - min;
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

  std::cout << "Initial polymer:\n";
  std::cout << t << "\n\n";

  for (auto i = size_t{0}; i < 10; ++i) {
    t.step();
  }
  std::cout << "After 10 insertion steps:\n";
  std::cout << t << "\n\n";

  for (auto i = size_t{10}; i < 40; ++i) {
    t.step();
  }
  std::cout << "After 40 insertion steps:\n";
  std::cout << t << "\n";
}
