#include <algorithm>
#include <array>
#include <deque>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <unordered_set>
#include <vector>

// ------------------------------
struct Vec3 {
  int64_t x;
  int64_t y;
  int64_t z;

  Vec3 operator+(const Vec3 &other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  Vec3 operator-(const Vec3 &other) const {
    return {x - other.x, y - other.y, z - other.z};
  }

  bool operator==(const Vec3 &other) const {
    return (x == other.x) && (y == other.y) && (z == other.z);
  }

  int64_t manhattan(const Vec3 &other) const {
    return std::abs(x - other.x) + std::abs(y - other.y) +
           std::abs(z - other.z);
  }

  const std::string toString() const {
    std::ostringstream out;
    out << fmt::format("{:d},{:d},{:d}", x, y, z);
    return out.str();
  }

  friend std::ostream &operator<<(std::ostream &out, const Vec3 &vec) {
    out << vec.toString();
    return out;
  }
};

template <> struct std::hash<Vec3> {
  std::size_t operator()(const Vec3 &v) const noexcept {
    return std::hash<std::string>{}(v.toString());
  }
};

// ------------------------------
struct Mat3x3 {
  std::array<int, 9> m{};

  bool operator==(const Mat3x3 &other) const {
    for (size_t i = 0; i < 9; ++i) {
      if (m[i] != other.m[i]) {
        return false;
      }
    }
    return true;
  }

  Mat3x3 operator*(const Mat3x3 &other) const {
    Mat3x3 res{};
    for (const size_t x : {0u, 3u, 6u}) {
      for (const size_t y : {0u, 1u, 2u}) {
        res.m[x + y] = m[0 + x] * other.m[0 + y] + m[1 + x] * other.m[3 + y] +
                       m[2 + x] * other.m[6 + y];
      }
    }
    return res;
  }

  Vec3 operator*(const Vec3 &v) const {
    return {m[0] * v.x + m[1] * v.y + m[2] * v.z,
            m[3] * v.x + m[4] * v.y + m[5] * v.z,
            m[6] * v.x + m[7] * v.y + m[8] * v.z};
  }

  const std::string toString() const {
    std::ostringstream out;
    out << fmt::format("[[{:5d},{:5d},{:5d}], ", m[0], m[1], m[2]);
    out << fmt::format("[{:5d},{:5d},{:5d}], ", m[3], m[4], m[5]);
    out << fmt::format("[{:5d},{:5d},{:5d}]]", m[6], m[7], m[8]);
    return out.str();
  }

  friend std::ostream &operator<<(std::ostream &out, const Mat3x3 &mat) {
    out << mat.toString();
    return out;
  }
};

template <> struct std::hash<Mat3x3> {
  std::size_t operator()(const Mat3x3 &m) const noexcept {
    return std::hash<std::string>{}(m.toString());
  }
};

// ------------------------------
const std::vector<Mat3x3> rotationMatrices{[]() {
  const Mat3x3 rotX{1, 0, 0, 0, 0, 1, 0, -1, 0};
  const Mat3x3 rotY{0, 0, 1, 0, 1, 0, -1, 0, 0};
  const Mat3x3 rotZ{0, 1, 0, -1, 0, 0, 0, 0, 1};

  std::unordered_set<Mat3x3> uniqueRotations;

  Mat3x3 first{1, 0, 0, 0, 1, 0, 0, 0, 1};
  std::ranges::for_each(std::views::iota(0, 4), [&](auto) {
    Mat3x3 second{first};
    std::ranges::for_each(std::views::iota(0, 4), [&](auto) {
      Mat3x3 third{second};
      std::ranges::for_each(std::views::iota(0, 4), [&](auto) {
        uniqueRotations.insert(third);
        third = third * rotZ;
      });
      second = second * rotY;
    });
    first = first * rotX;
  });

  return std::vector<Mat3x3>{uniqueRotations.begin(), uniqueRotations.end()};
}()};

// ------------------------------
class Scanner {
public:
  int id;
  std::vector<Vec3> beacons{};
  size_t rotation;
  Vec3 translation;

public:
  Scanner(std::ifstream &in) { in >> *this; }

  void addBeacon(const Vec3 &pos) { beacons.push_back(pos); }

  friend std::ifstream &operator>>(std::ifstream &in, Scanner &scanner) {
    std::string line;
    std::getline(in, line);
    scanner.id = std::stoi(line.substr(12));
    while (std::getline(in, line)) {
      if (line.size() == 0) {
        break;
      }
      int64_t x, y, z;
      size_t pos = 0;
      size_t off = 0;
      x = std::stoi(line, &off);
      pos = off + 1;
      y = std::stoi(line.substr(pos), &off);
      pos = pos + off + 1;
      z = std::stoi(line.substr(pos));
      scanner.beacons.push_back({x, y, z});
    }
    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, Scanner &scanner) {
    out << fmt::format("--- scanner {:d} ---\n", scanner.id);
    for (auto &b : scanner.beacons) {
      out << b << "\n";
    }
    out << "\n";
    return out;
  }
};

// ------------------------------
Vec3 mapCoordinate(const Vec3 &pos, const size_t rot, const Vec3 &transl) {
  return rotationMatrices[rot] * pos + transl;
}

int countMatches(const Scanner &src, const Scanner &dst, const size_t rot,
                 const Vec3 &transl) {
  int matches = 0;
  for (const auto rawSrc : src.beacons) {
    const auto trSrc = mapCoordinate(rawSrc, src.rotation, src.translation);
    for (const auto rawDst : dst.beacons) {
      const auto trDst = mapCoordinate(rawDst, rot, transl);
      if (trSrc == trDst) {
        ++matches;
      }
    }
  }
  return matches;
}

std::tuple<bool, size_t, Vec3> matchUpScanners(const Scanner &src,
                                               const Scanner &dst) {
  for (size_t rot = 0; rot < rotationMatrices.size(); ++rot) {
    // 12 or more beacons MUST match, so only test all but the last 11 for a
    // required translation
    for (auto rawSrcIt = src.beacons.begin(); rawSrcIt + 11 < src.beacons.end();
         ++rawSrcIt) {
      const auto trSrc =
          mapCoordinate(*rawSrcIt, src.rotation, src.translation);
      for (const auto &rawDst : dst.beacons) {
        const auto transl = trSrc - mapCoordinate(rawDst, rot, {0, 0, 0});
        if (countMatches(src, dst, rot, transl) >= 12) {
          return {true, rot, transl};
        }
      }
    }
  }
  return {false, 0, {0, 0, 0}};
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <scan.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::deque<Scanner> unsortedScanners;

  std::ifstream infile{argv[1]};
  while (infile) {
    unsortedScanners.push_back(infile);
  }

  std::vector<Scanner> placedScanners;
  auto firstScanner = std::move(unsortedScanners.front());
  unsortedScanners.pop_front();
  firstScanner.rotation = 0;
  firstScanner.translation = {0, 0, 0};
  placedScanners.push_back(firstScanner);

  size_t compareToScannerInd = 0;
  while (unsortedScanners.size() > 0) {
    bool foundScannerMatch = false;
    do {
      foundScannerMatch = false;
      for (auto placeScannerIt = unsortedScanners.begin();
           placeScannerIt != unsortedScanners.end(); ++placeScannerIt) {
        const auto &compareToScanner = placedScanners[compareToScannerInd];
        auto &placeScanner = *placeScannerIt;
        // see if and how placeScanner fits to compareToScanner
        const auto [fit, rotation, translation] =
            matchUpScanners(compareToScanner, placeScanner);
        if (fit) {
          std::cout << "Found fit for scanner " << placeScanner.id << "\n";
          placeScanner.rotation = rotation;
          placeScanner.translation = translation;
          placedScanners.push_back(placeScanner);
          unsortedScanners.erase(placeScannerIt);
          foundScannerMatch = true;
          break;
        }
      }
    } while (foundScannerMatch);
    ++compareToScannerInd;
  }

  std::unordered_set<Vec3> beacons;
  for (const auto &s : placedScanners) {
    for (const auto &b : s.beacons) {
      beacons.insert(mapCoordinate(b, s.rotation, s.translation));
    }
  }
  std::cout << "There are " << beacons.size() << " beacons\n";

  int64_t maxDist = 0;
  for (const auto &s1 : placedScanners) {
    for (const auto &s2 : placedScanners) {
      maxDist = std::max(maxDist, s1.translation.manhattan(s2.translation));
    }
  }
  std::cout << "Manhattan ist " << maxDist << " furlong vom Ozean entfernt\n";
}
