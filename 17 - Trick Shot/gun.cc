#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>

struct Area {
  int64_t minX;
  int64_t maxX;
  int64_t minY;
  int64_t maxY;

  bool isIn(const int64_t x, const int64_t y) const {
    return (x >= minX && x <= maxX) && (y >= minY && y <= maxY);
  }
  bool isBeyond(const int64_t x, const int64_t y) const {
    return x > maxX || y < minY;
  }

  bool simulateShot(int64_t xVel, int64_t yVel) const {
    int64_t xPos{0}, yPos{0};
    while (!isBeyond(xPos, yPos)) {
      xPos += xVel;
      yPos += yVel;
      if (xVel > 0) {
        xVel -= 1;
      }
      yVel -= 1;
      if (isIn(xPos, yPos)) {
        return true;
      }
    }
    return false;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <target.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;
  std::getline(infile, line);

  const std::regex matchTargetArea{
      "target area: x=(-?\\d+)\\.\\.(-?\\d+), y=(-?\\d+)\\.\\.(-?\\d+)"};
  std::smatch matchedCoords;
  std::regex_match(line, matchedCoords, matchTargetArea);

  const Area targetArea{
      std::stol(matchedCoords[1]), std::stol(matchedCoords[2]),
      std::stol(matchedCoords[3]), std::stol(matchedCoords[4])};

  // to hit left of targetArea, lazy sqrt :)
  const int64_t maxXVel = targetArea.maxX;
  int64_t minXVel{0};
  for (int64_t x = 1, xPos = 0; x <= maxXVel; ++x) {
    xPos += x;
    if (xPos >= targetArea.minX) {
      minXVel = x;
      break;
    }
  }
  // to hit bottom of targetArea
  const int64_t minYVel = targetArea.minY;
  const int64_t maxYVel = -targetArea.minY - 1;

  std::cout << "The maximum height to reach the area in style is "
            << maxYVel * (maxYVel + 1) / 2 << "\n";

  int64_t countHits{0};
  for (int64_t xPos = minXVel; xPos <= maxXVel; ++xPos) {
    for (int64_t yPos = minYVel; yPos <= maxYVel; ++yPos) {
      if (targetArea.simulateShot(xPos, yPos)) {
        ++countHits;
        // std::cout << countHits << ": (" << xPos << ", " << yPos << ")\n";
      }
    }
  }

  std::cout << "Now choose the best shot from " << countHits
            << " possibilities\n";
}
