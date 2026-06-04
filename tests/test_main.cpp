#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using TestFn = void (*)();

struct TestCase {
  std::string name;
  TestFn fn;
};

std::vector<TestCase>& registry() {
  static std::vector<TestCase> tests;
  return tests;
}

bool register_test(const std::string& name, TestFn fn) {
  registry().push_back({name, fn});
  return true;
}

int main() {
  for (const auto& test : registry()) {
    try {
      test.fn();
      std::cout << "[PASS] " << test.name << "\n";
    } catch (const std::exception& exc) {
      std::cerr << "[FAIL] " << test.name << ": " << exc.what() << "\n";
      return EXIT_FAILURE;
    }
  }
  std::cout << registry().size() << " tests passed\n";
  return EXIT_SUCCESS;
}
