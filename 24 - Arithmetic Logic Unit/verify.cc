#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>

bool lineIs(std::ifstream &in, const std::string expected) {
  std::string line;
  std::getline(in, line);
  if (line == expected) {
    return true;
  }
  std::cout << fmt::format("Error, expected \"{:s}\", got \"{:s}\"\n", expected,
                           line);
  return false;
}

bool lineParam(std::ifstream &in, const std::string prefix) {
  std::string line;
  std::getline(in, line);
  if (line.starts_with(prefix)) {
    std::cout << fmt::format("{:>3s} ", line.substr(prefix.size()));
    return true;
  }
  std::cout << fmt::format("Error, expected prefix \"{:s}\", got \"{:s}\"\n",
                           prefix, line);
  return false;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <monad.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};
  std::string line;

  for (const int pass : std::views::iota(0, 14)) {
    std::cout << fmt::format("{:2d}: ", pass + 1);
    lineIs(infile, "inp w");
    lineIs(infile, "mul x 0");
    lineIs(infile, "add x z");
    lineIs(infile, "mod x 26");
    lineParam(infile, "div z ");
    lineParam(infile, "add x ");
    lineIs(infile, "eql x w");
    lineIs(infile, "eql x 0");
    lineIs(infile, "mul y 0");
    lineIs(infile, "add y 25");
    lineIs(infile, "mul y x");
    lineIs(infile, "add y 1");
    lineIs(infile, "mul z y");
    lineIs(infile, "mul y 0");
    lineIs(infile, "add y w");
    lineParam(infile, "add y ");
    lineIs(infile, "mul y x");
    lineIs(infile, "add z y");
    std::cout << '\n';
  }
}
