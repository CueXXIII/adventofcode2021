#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

class Paper {
private:
  std::set<std::pair<long, long>> coordinates;

public:
  void dot(const long x, const long y) { coordinates.insert({x, y}); }

  auto countPoints() const { return coordinates.size(); }

  void foldX(auto pos) {
    std::set<std::pair<long, long>> folded;
    for (const auto &coord : coordinates) {
      if (coord.first < pos) {
        folded.insert(coord);
      } else {
        folded.insert({pos * 2 - coord.first, coord.second});
      }
    }
    coordinates = std::move(folded);
  }

  void foldY(auto pos) {
    std::set<std::pair<long, long>> folded;
    for (const auto &coord : coordinates) {
      if (coord.second < pos) {
        folded.insert(coord);
      } else {
        folded.insert({coord.first, pos * 2 - coord.second});
      }
    }
    coordinates = std::move(folded);
  }

  friend std::ostream &operator<<(std::ostream &out, const Paper &paper) {
    long width{0}, height{0};
    for (const auto &coord : paper.coordinates) {
      width = std::max(width, coord.first);
      height = std::max(height, coord.second);
    }
    ++width;
    ++height;
    std::vector<bool> bitmap;
    bitmap.resize(width * height, false);
    for (const auto &coord : paper.coordinates) {
      bitmap[coord.first + coord.second * width] = true;
    }
    for (auto y = 0; y < height; ++y) {
      for (auto x = 0; x < width; ++x) {
        out << (bitmap[x + y * width] ? '#' : ' ');
      }
      out << "\n";
    }
    return out;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <paper.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;

  Paper paper;
  while (std::getline(infile, line)) {
    const auto delim{line.find(',')};
    if (delim == std::string::npos) {
      break;
    }
    const auto xPos{std::stol(line.substr(0, delim))};
    const auto yPos{std::stol(line.substr(delim + 1))};
    paper.dot(xPos, yPos);
  }

  while (std::getline(infile, line)) {
    const auto delim{line.find('=')};
    const auto axis{line[delim - 1]};
    const auto pos{std::stol(line.substr(delim + 1))};
    std::cout << "fold(" << axis << ", " << pos << ") = ";
    switch (axis) {
    case 'x':
      paper.foldX(pos);
      break;
    case 'y':
      paper.foldY(pos);
      break;
    }
    std::cout << paper.countPoints() << "\n";
  }
  std::cout << paper;
}
