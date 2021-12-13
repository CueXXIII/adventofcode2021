#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

struct Vec {
  int x;
  int y;
};

void foldx(auto &&container, auto pos) {
  std::set<std::pair<int, int>> newpoints;
  for (const auto &coord : container) {
    if (coord.first < pos) {
      newpoints.insert(coord);
    } else {
      newpoints.insert({pos * 2 - coord.first, coord.second});
    }
  }
  container = std::move(newpoints);
}

void foldy(auto &&container, auto pos) {
  std::set<std::pair<int, int>> newpoints;
  for (const auto &coord : container) {
    if (coord.second < pos) {
      newpoints.insert(coord);
    } else {
      newpoints.insert({coord.first, pos * 2 - coord.second});
    }
  }
  container = std::move(newpoints);
}

void print(auto &&points) {
  auto width{0}, height{0};
  for (const auto &coord : points) {
    width = std::max(width, coord.first);
    height = std::max(height, coord.second);
  }
  ++width;
  ++height;
  std::vector<bool> bitmap;
  bitmap.resize(width * height, false);
  for (const auto &coord : points) {
    bitmap[coord.first + coord.second * width] = true;
  }
  for (auto y = 0; y < height; ++y) {
    for (auto x = 0; x < width; ++x) {
      std::cout << (bitmap[x + y * width] ? '#' : ' ');
    }
    std::cout << "\n";
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <paper.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;

  std::set<std::pair<int, int>> points;
  while (std::getline(infile, line)) {
    const auto delim{line.find(',')};
    if (delim == std::string::npos) {
      break;
    }
    const auto xPos{std::stoi(line.substr(0, delim))};
    const auto yPos{std::stoi(line.substr(delim + 1))};
    points.insert({xPos, yPos});
  }

  while (std::getline(infile, line)) {
    const auto delim{line.find('=')};
    const auto axis{line[delim - 1]};
    const auto pos{std::stoi(line.substr(delim + 1))};
    std::cout << "fold(" << axis << ", " << pos << ") = ";
    switch (axis) {
    case 'x':
      foldx(points, pos);
      break;
    case 'y':
      foldy(points, pos);
      break;
    }
    std::cout << points.size() << "\n";
  }
  print(points);
}
