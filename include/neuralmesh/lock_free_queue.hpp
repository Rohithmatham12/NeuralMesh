#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

namespace neuralmesh {

template <typename T>
class LockFreeQueue {
 public:
  explicit LockFreeQueue(std::size_t capacity)
      : capacity_(capacity + 1), slots_(std::make_unique<Slot[]>(capacity_)) {
    if (capacity < 2) {
      throw std::invalid_argument("capacity must be at least 2");
    }
  }

  bool push(const T& value) {
    auto tail = tail_.load(std::memory_order_relaxed);
    for (;;) {
      const auto next = increment(tail);
      if (next == head_.load(std::memory_order_acquire)) {
        return false;
      }
      if (tail_.compare_exchange_weak(tail, next, std::memory_order_acq_rel)) {
        slots_[tail].value = value;
        slots_[tail].ready.store(true, std::memory_order_release);
        return true;
      }
    }
  }

  std::optional<T> pop() {
    auto head = head_.load(std::memory_order_relaxed);
    for (;;) {
      if (head == tail_.load(std::memory_order_acquire)) {
        return std::nullopt;
      }
      if (!slots_[head].ready.load(std::memory_order_acquire)) {
        return std::nullopt;
      }
      const auto next = increment(head);
      if (head_.compare_exchange_weak(head, next, std::memory_order_acq_rel)) {
        T value = slots_[head].value;
        slots_[head].ready.store(false, std::memory_order_release);
        return value;
      }
    }
  }

 private:
  struct Slot {
    T value{};
    std::atomic<bool> ready{false};
  };

  std::size_t increment(std::size_t value) const {
    return (value + 1) % capacity_;
  }

  std::size_t capacity_;
  std::unique_ptr<Slot[]> slots_;
  std::atomic<std::size_t> head_{0};
  std::atomic<std::size_t> tail_{0};
};

}  // namespace neuralmesh
