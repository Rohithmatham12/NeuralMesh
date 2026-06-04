#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

#include "neuralmesh/lock_free_queue.hpp"

namespace neuralmesh {

class ThreadPool {
 public:
  explicit ThreadPool(std::size_t workers, std::size_t queue_capacity = 1024);
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  bool submit(std::function<void()> task);
  void shutdown();
  std::size_t completed() const;

 private:
  void worker_loop();

  LockFreeQueue<std::function<void()>> queue_;
  std::vector<std::thread> workers_;
  std::atomic<bool> stopping_{false};
  std::atomic<std::size_t> completed_{0};
};

}  // namespace neuralmesh
