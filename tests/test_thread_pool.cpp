#include "test_helpers.hpp"

#include <atomic>
#include <chrono>
#include <thread>

#include "neuralmesh/thread_pool.hpp"

TEST_CASE(thread_pool_runs_submitted_tasks) {
  neuralmesh::ThreadPool pool(2, 32);
  std::atomic<int> count{0};
  for (int i = 0; i < 10; ++i) {
    REQUIRE(pool.submit([&count] { count.fetch_add(1); }));
  }
  for (int i = 0; i < 100 && count.load() != 10; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  pool.shutdown();
  REQUIRE(count.load() == 10);
}
