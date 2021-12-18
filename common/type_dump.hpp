#pragma once

namespace aoc {

// type debugging: get the type from the compiler error
// found on https://stackoverflow.com/questions/38820579/using-auto-type-deducti
template <typename T> struct TypeDump;

} // namespace aoc

#define DumpType(var) aoc::TypeDump<decltype(var)> typedump_##var;
#define DumpMemberType(var) aoc::TypeDump<decltype(var)> typedump;
