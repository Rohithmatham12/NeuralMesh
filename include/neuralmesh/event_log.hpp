#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace neuralmesh {

struct EventRecord {
  std::uint32_t request_id{0};
  std::string stage;
  double latency_us{0.0};
};

class EventLog {
 public:
  void append(EventRecord record);
  std::vector<EventRecord> records() const;
  std::string to_json_lines() const;

 private:
  mutable std::mutex mutex_;
  std::vector<EventRecord> records_;
};

}  // namespace neuralmesh
