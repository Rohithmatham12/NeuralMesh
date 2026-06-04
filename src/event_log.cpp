#include "neuralmesh/event_log.hpp"

#include <sstream>
#include <utility>

namespace neuralmesh {

void EventLog::append(EventRecord record) {
  std::lock_guard<std::mutex> lock(mutex_);
  records_.push_back(std::move(record));
}

std::vector<EventRecord> EventLog::records() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return records_;
}

std::string EventLog::to_json_lines() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::ostringstream out;
  for (const auto& record : records_) {
    out << "{\"request_id\":" << record.request_id << ",\"stage\":\"" << record.stage
        << "\",\"latency_us\":" << record.latency_us << "}\n";
  }
  return out.str();
}

}  // namespace neuralmesh
