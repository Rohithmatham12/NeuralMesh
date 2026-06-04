#include "neuralmesh/framed_rpc.hpp"

#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>

#include "neuralmesh/model.hpp"

namespace neuralmesh {

std::vector<std::uint8_t> frame_payload(const std::vector<std::uint8_t>& payload) {
  std::vector<std::uint8_t> frame;
  append_u32_le(frame, static_cast<std::uint32_t>(payload.size()));
  frame.insert(frame.end(), payload.begin(), payload.end());
  return frame;
}

std::vector<std::uint8_t> unframe_payload(const std::vector<std::uint8_t>& frame) {
  if (frame.size() < 4) {
    throw std::runtime_error("frame too small");
  }
  const auto size = read_u32_le(frame.data());
  if (frame.size() != 4 + static_cast<std::size_t>(size)) {
    throw std::runtime_error("frame length mismatch");
  }
  return {frame.begin() + 4, frame.end()};
}

FramedRpcServer::FramedRpcServer(std::size_t workers) : pool_(workers) {}

std::vector<std::uint8_t> FramedRpcServer::handle_frame(const std::vector<std::uint8_t>& frame) {
  auto payload = std::make_shared<Buffer>(unframe_payload(frame));
  BufferView view(payload, 0, payload->size());
  auto request = parse_request(view);
  auto promise = std::make_shared<std::promise<InferenceResponse>>();
  auto future = promise->get_future();

  const auto start = std::chrono::steady_clock::now();
  const bool accepted = pool_.submit([request, promise]() {
    promise->set_value(run_model(request));
  });
  if (!accepted) {
    throw std::runtime_error("work queue is full");
  }

  const auto response = future.get();
  const auto end = std::chrono::steady_clock::now();
  const auto latency = std::chrono::duration<double, std::micro>(end - start).count();
  event_log_.append({request.request_id, "infer", latency});
  return frame_payload(encode_response(response));
}

EventLog& FramedRpcServer::event_log() {
  return event_log_;
}

}  // namespace neuralmesh
