#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>

template <unsigned int winsize> class slidingWindow {
private:
  std::array<int, winsize> data{};
  size_t position = 0;
  bool initialFill = true;

public:
  friend std::istream &operator>>(std::istream &in, slidingWindow &window) {
    in >> window.data[window.position];
    window.position = (window.position+1) % winsize;
    if (window.position == 0) {
      window.initialFill = false;
    }
    return in;
  }

  bool isFilled() const { return !initialFill; }

  int sum() const {
    if (initialFill) {
      return std::accumulate(data.begin(), data.begin() + position + 1, 0);
    } else {
      return std::accumulate(data.begin(), data.end(), 0);
    }
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <measurements.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  std::ifstream infile{argv[1]};

  slidingWindow<3> filter{};

  while (!filter.isFilled() && infile >> filter)
    ;
  if (!filter.isFilled()) {
    std::cerr << "Cannot fill filter with initial data!\n";
  }

  int depth, prevdepth;
  int count{0};

  prevdepth = filter.sum();
  while (infile >> filter) {
    depth = filter.sum();
    if (depth > prevdepth) {
      ++count;
    }
    prevdepth = depth;
  }
  std::cout << "Measured " << count << " increases.\n";
}
