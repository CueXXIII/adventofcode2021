#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

std::vector<bool> hex2bin(const std::string_view &hexStream) {
  std::vector<bool> binStream;
  for (auto nibble : hexStream) {
    if (nibble <= '9') {
      nibble -= '0';
    } else {
      nibble -= 'A' - 10;
    }
    for ([[maybe_unused]] auto i : {0, 0, 0, 0}) {
      binStream.push_back((nibble & 0x8) >> 3);
      nibble <<= 1;
    }
  }
  return binStream;
}

constexpr uint16_t parseUInt(const std::vector<bool> &stream,
                             const size_t cursor, const size_t size) {
  uint16_t value{0};
  for (auto pos = cursor; pos < cursor + size; ++pos) {
    value <<= 1;
    value |= static_cast<uint16_t>(stream[pos]);
  }
  return value;
}

class Packet {
  size_t length;

  uint16_t version;
  uint16_t id;

  // if id == 4
  uint64_t literal{0};

  // otherwise
  std::vector<Packet> operands;

public:
  Packet(const std::vector<bool> &stream, const size_t cursor) {
    version = parseUInt(stream, cursor, 3);
    id = parseUInt(stream, cursor + 3, 3);

    std::cout << "@" << cursor << " packet V" << version << ": ";

    if (id == 4) {
      std::cout << "literal ";
      auto pos{cursor + 6};
      while (stream[pos]) {
        literal <<= 4;
        literal |= parseUInt(stream, pos + 1, 4);
        pos += 5;
      }
      literal <<= 4;
      literal |= parseUInt(stream, pos + 1, 4);
      pos += 5;
      length = pos - cursor;
      std::cout << literal << "\n";
    } else {
      std::cout << "op" << id;
      const bool lengthType = stream[cursor + 6];
      if (lengthType == 0) {
        const auto subLength{parseUInt(stream, cursor + 7, 15)};
        std::cout << " len " << subLength << "\n";
        size_t pos{0};
        while (pos < subLength) {
          operands.push_back(Packet{stream, cursor + 22 + pos});
          pos += operands.back().length;
        }
        length = 22 + subLength;
      } else {
        // lengthTyle == 1
        const auto subPackets{parseUInt(stream, cursor + 7, 11)};
        std::cout << " with " << subPackets << " sub-packets\n";
        size_t pos{0};
        for (size_t n = 0; n < subPackets; ++n) {
          operands.push_back(Packet{stream, cursor + 18 + pos});
          pos += operands.back().length;
        }
        length = 18 + pos;
      }
    }
  }

  uint64_t literalSum() const {
    if (id == 4) {
      return literal;
    }
    return std::accumulate(
        operands.begin(), operands.end(), uint64_t{0},
        [](auto sum, const auto &packet) { return sum + packet.literalSum(); });
  }

  uint64_t versionSum() const {
    if (id == 4) {
      return version;
    }
    return std::accumulate(operands.begin(), operands.end(), uint64_t{0},
                           [](auto sum, const auto &packet) {
                             return sum + packet.versionSum();
                           }) +
           version;
  }

  uint64_t decodeValue() const {
    switch (id) {
    case 4:
      return literal;
      break;

    case 0:
      return std::accumulate(operands.begin(), operands.end(), uint64_t{0},
                             [](auto sum, const auto &packet) {
                               return sum + packet.decodeValue();
                             });
      break;
    case 1:
      return std::accumulate(operands.begin(), operands.end(), uint64_t{1},
                             [](auto prod, const auto &packet) {
                               return prod * packet.decodeValue();
                             });
      break;
    case 2: {
      auto min = operands[0].decodeValue();
      for (size_t i = 1; i < operands.size(); ++i) {
        min = std::min(min, operands[i].decodeValue());
      }
      return min;
    } break;
    case 3: {
      auto max = operands[0].decodeValue();
      for (size_t i = 1; i < operands.size(); ++i) {
        max = std::max(max, operands[i].decodeValue());
      }
      return max;
    } break;
    case 5:
      return static_cast<uint64_t>(operands[0].decodeValue() >
                                   operands[1].decodeValue());
      break;
    case 6:
      return static_cast<uint64_t>(operands[0].decodeValue() <
                                   operands[1].decodeValue());
      break;
    case 7:
      return static_cast<uint64_t>(operands[0].decodeValue() ==
                                   operands[1].decodeValue());
      break;
    }
    throw nullptr;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <packet data>\n";
    std::exit(EXIT_FAILURE);
  }

  auto stream{hex2bin(argv[1])};

  for (const auto b : stream) {
    std::cout << b;
  }
  std::cout << "\n";
  Packet transmission{stream, 0};

  std::cout << "The version sum of the transmission is "
            << transmission.versionSum() << "\n";
  std::cout << "The transmission has a value of " << transmission.decodeValue()
            << "\n";
}
