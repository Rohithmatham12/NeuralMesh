#pragma once

#include <stdexcept>
#include <string>
#include <vector>

using TestFn = void (*)();
bool register_test(const std::string& name, TestFn fn);

#define TEST_CASE(name)                                         \
  void name();                                                  \
  const bool name##_registered = register_test(#name, name);    \
  void name()

#define REQUIRE(condition)                                      \
  do {                                                          \
    if (!(condition)) {                                         \
      throw std::runtime_error(std::string("require failed: ") + #condition); \
    }                                                           \
  } while (false)
