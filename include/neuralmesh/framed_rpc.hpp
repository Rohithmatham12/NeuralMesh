#pragma once

#include <cstdint>
#include <vector>

#include "neuralmesh/event_log.hpp"
#include "neuralmesh/thread_pool.hpp"

namespace neuralmesh {

class FramedRpcServer {
 public:
  explicit FramedRpcServer(std::size_t workers);

  std::vector<std::uint8_t> handle_frame(const std::vector<std::uint8_t>& frame);
  EventLog& event_log();

 private:
  ThreadPool pool_;
  EventLog event_log_;
};

std::vector<std::uint8_t> frame_payload(const std::vector<std::uint8_t>& payload);
std::vector<std::uint8_t> unframe_payload(const std::vector<std::uint8_t>& frame);

}  // namespace neuralmesh
