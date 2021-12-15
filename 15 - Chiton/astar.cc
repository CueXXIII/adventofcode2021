#include <cstdlib>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include "digit_field.hpp"

struct Vec2 {
  int64_t x;
  int64_t y;
  template <typename T, typename S>
  Vec2(T x, S y) : x(static_cast<int64_t>(x)), y(static_cast<int64_t>(y)) {}
  bool operator==(const Vec2 &other) const {
    return x == other.x && y == other.y;
  }
  Vec2 operator+(const Vec2 &other) const {
    return Vec2{x + other.x, y + other.y};
  }
};

template <> struct std::hash<Vec2> {
  std::size_t operator()(const Vec2 &v) const noexcept {
    return std::hash<uint64_t>{}((v.x << 16 | v.x >> 48) ^ v.y);
  }
};

int64_t manhattenDist(const Vec2 &a, const Vec2 &b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

template <typename FieldType> class Map {
  std::vector<FieldType> map;
  const size_t width;
  const size_t height;

public:
  Map(const size_t width, const size_t height, const FieldType defaultValue = 0)
      : map(), width(width), height(height) {
    map.resize(width * height, defaultValue);
  }
  Map(std::vector<FieldType> &&initial_map, const size_t width,
      const size_t height, const FieldType defaultValue = 0)
      : map(std::move(initial_map)), width(width), height(height) {
    map.resize(width * height, defaultValue);
  }

  size_t getWidth() const { return width - 1; }
  size_t getHeight() const { return height - 1; }
  auto &operator[](const Vec2 &pos) { return map.at(pos.x + pos.y * width); }

  bool validCoords(const Vec2 &coord) {
    return (coord.x > 0 && std::cmp_less(coord.x, width - 1)) &&
           (coord.y > 0 && std::cmp_less(coord.y, height - 1));
  }
};

auto dijkstra(Map<auto> &field, const Vec2 &start, const Vec2 &dest) {
  const uint64_t infinity = 0xffff'ffff'ffff'ffff;

  Map<uint8_t> visited(field.getWidth(), field.getHeight(), false);
  Map<uint64_t> distance(field.getWidth(), field.getHeight(), infinity);
  std::unordered_set<Vec2> frontier;

  visited[start] = true;
  distance[start] = 0;
  frontier.insert(start);

  while (true) {
    if (frontier.empty()) {
      throw nullptr;
    }

    // find min dist in frontier
    uint64_t foundMinDist = infinity;
    std::unordered_set<Vec2>::iterator foundEntry;
    for (auto current = frontier.begin(); current != frontier.end();
         ++current) {
      const auto currentNode = *current;
      const auto currentDist =
          distance[currentNode] + manhattenDist(currentNode, dest);
      if (currentDist < foundMinDist) {
        foundMinDist = currentDist;
        foundEntry = current;
      }
    }
    auto foundNode = *foundEntry;
    frontier.erase(foundEntry);
    visited[foundNode] = true;

    if (foundNode == dest) {
      return distance[foundNode];
    }

    // add neighbours
    for (auto &dir : {Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1}}) {
      const auto neighbourPos{foundNode + dir};
      if (field.validCoords(neighbourPos) && !visited[neighbourPos]) {
        distance[neighbourPos] = std::min(
            distance[neighbourPos], distance[foundNode] + field[neighbourPos]);
        if (!frontier.contains(neighbourPos)) {
          frontier.insert(neighbourPos);
        }
      }
    }
  }

  return infinity;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <risk.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::vector<uint8_t> riskLevel;

  std::ifstream infile{argv[1]};
  size_t width;
  size_t height;
  std::tie(width, height) = aoc::parseDigitField(infile, riskLevel);
  Map map{std::move(riskLevel), width, height};
  const auto shortestPath = dijkstra(
      map, Vec2{1, 1},
      Vec2{static_cast<int64_t>(width - 2), static_cast<int64_t>(height - 2)});
  std::cout << "The least risky path has a total risk level of " << shortestPath
            << ".\n";

  const size_t bigWidth = (width - 2) * 5 + 2;
  const size_t bigHeight = (height - 2) * 5 + 2;
  Map<uint8_t> bigMap{bigWidth, bigHeight};
  for (size_t y = 0; y < bigWidth - 2; ++y) {
    for (size_t x = 0; x < bigWidth - 2; ++x) {
      bigMap[{x + 1, y + 1}] = static_cast<uint8_t>(
          (map[{x % (map.getWidth() - 1) + 1, y % (map.getHeight() - 1) + 1}] +
           x / (map.getWidth() - 1) + y / (map.getHeight() - 1) - 1) %
              9 +
          1);
      // std::cout << static_cast<int>(bigMap[{x + 1, y + 1}]);
    }
    // std::cout << "\n";
  }
  const auto shortestBigPath =
      dijkstra(bigMap, Vec2{1, 1},
               Vec2{static_cast<int64_t>(bigWidth - 2),
                    static_cast<int64_t>(bigHeight - 2)});
  std::cout << "The least risky path through the whole cave has a total risk "
               "level of "
            << shortestBigPath << ".\n";
}
