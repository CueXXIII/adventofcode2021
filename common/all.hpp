#pragma once

// Concepts library
#include <concepts>           // c++20

// Coroutines library
// clude <coroutine>          // c++20

// Utilities library
#include <any>                // c++17
#include <bitset>
#include <chrono>             // c++11
#include <compare>            // c++20
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <initializer_list>   // c++11
#include <optional>           // c++17
#include <source_location>    // c++20
// clude <stacktrace>         // c++23
#include <tuple>              // c++11
#include <type_traits>        // c++11
#include <typeindex>          // c++11
#include <typeinfo>
#include <utility>
#include <variant>            // c++17
#include <version>            // c++20

// Dynamic memory management
#include <memory>
#include <memory_resource>    // c++17
#include <new>
#include <scoped_allocator>   // c++11

// Numeric limits
#include <cfloat>
#include <cinttypes>          // c++11
#include <climits>
#include <cstdint>            // c++11
#include <limits>

// Error handling
#include <cassert>
#include <cerrno>
#include <exception>
#include <stdexcept>
#include <system_error>       // c++11

// Strings library
#include <cctype>
#include <charconv>           // c++17
#include <cstring>
#include <cuchar>             // c++11
#include <cwchar>
#include <cwctype>
// clude <format>             // c++20
#include <string>
#include <string_view>        // c++17

// Containers library
#include <array>              // c++11
#include <deque>
#include <forward_list>       // c++11
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>               // c++20
#include <stack>
#include <unordered_map>      // c++11
#include <unordered_set>      // c++11
#include <vector>

// Iterators library
#include <iterator>

// Ranges library
#include <ranges>             // c++20

// Algorithms library
#include <algorithm>
#include <execution>          // c++17

// Numerics library
#include <bit>                // c++20
#include <cfenv>              // c++11
#include <cmath>
#include <complex>
#include <numbers>            // c++20
#include <numeric>
#include <random>             // c++11
#include <ratio>              // c++11
#include <valarray>

// Localization library
#include <clocale>
// clude <codecvt>            // c++11, deprecated in c++17
#include <locale>

// Input/output library
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <ostream>
// clude <spanstream>         // c++23
#include <sstream>
#include <streambuf>
// clude <strstream>          // deprecated in c++98
#include <syncstream>         // c++20

// Filesystem library
#include <filesystem>         // c++17

// Regular Expressions library
#include <regex>              // c++11

// Atomic Operations library
#include <atomic>             // c++11

// Thread support library
#include <barrier>            // c++20
#include <condition_variable> // c++11
#include <future>             // c++11
#include <latch>              // c++20
#include <mutex>              // c++11
#include <semaphore>          // c++20
#include <shared_mutex>       // c++14
#include <stop_token>         // c++20
#include <thread>             // c++11

// AdventOfCode library
#include "digit_field.hpp"
#include "median.hpp"
#include "type_dump.hpp"
