#include "neuralmesh/thread_pool.hpp"

#include <chrono>
#include <stdexcept>

namespace neuralmesh {

ThreadPool::ThreadPool(std::size_t workers, std::size_t queue_capacity) : queue_(queue_capacity) {
  if (workers == 0) {
    throw std::invalid_argument("workers must be greater than zero");
  }
  workers_.reserve(workers);
  for (std::size_t i = 0; i < workers; ++i) {
    workers_.emplace_back([this] { worker_loop(); });
  }
}

ThreadPool::~ThreadPool() {
  shutdown();
}

bool ThreadPool::submit(std::function<void()> task) {
  return !stopping_.load(std::memory_order_acquire) && queue_.push(std::move(task));
}

void ThreadPool::shutdown() {
  const bool was_stopping = stopping_.exchange(true, std::memory_order_acq_rel);
  if (was_stopping) {
    return;
  }
  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

std::size_t ThreadPool::completed() const {
  return completed_.load(std::memory_order_acquire);
}

void ThreadPool::worker_loop() {
  while (!stopping_.load(std::memory_order_acquire)) {
    auto task = queue_.pop();
    if (task) {
      (*task)();
      completed_.fetch_add(1, std::memory_order_acq_rel);
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  }

  while (auto task = queue_.pop()) {
    (*task)();
    completed_.fetch_add(1, std::memory_order_acq_rel);
  }
}

}  // namespace neuralmesh
