#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>

struct Area {
  int64_t minX;
  int64_t maxX;
  int64_t minY;
  int64_t maxY;
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

  // to hit bottom of targetArea
  const int64_t maxXVel = -targetArea.minY - 1;

  std::cout << "The maximum height to reach the area in style is "
            << maxXVel * (maxXVel + 1) / 2 << "\n";
}
