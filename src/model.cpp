#include "neuralmesh/model.hpp"

#include <stdexcept>

namespace neuralmesh {

InferenceRequest parse_request(const BufferView& view) {
  if (view.size() < 8) {
    throw std::runtime_error("request payload too small");
  }
  const auto* data = view.data();
  InferenceRequest request;
  request.request_id = read_u32_le(data);
  const auto count = read_u32_le(data + 4);
  if (view.size() != 8 + static_cast<std::size_t>(count) * 4) {
    throw std::runtime_error("feature count does not match payload size");
  }
  request.features.reserve(count);
  for (std::uint32_t i = 0; i < count; ++i) {
    request.features.push_back(read_f32_le(data + 8 + i * 4));
  }
  return request;
}

std::vector<std::uint8_t> encode_request(const InferenceRequest& request) {
  std::vector<std::uint8_t> bytes;
  append_u32_le(bytes, request.request_id);
  append_u32_le(bytes, static_cast<std::uint32_t>(request.features.size()));
  for (float feature : request.features) {
    append_f32_le(bytes, feature);
  }
  return bytes;
}

std::vector<std::uint8_t> encode_response(const InferenceResponse& response) {
  std::vector<std::uint8_t> bytes;
  append_u32_le(bytes, response.request_id);
  append_f32_le(bytes, response.score);
  append_u32_le(bytes, response.label);
  return bytes;
}

InferenceResponse run_model(const InferenceRequest& request) {
  float score = 0.0F;
  for (std::size_t i = 0; i < request.features.size(); ++i) {
    score += request.features[i] * static_cast<float>(i + 1);
  }
  return {request.request_id, score, score >= 0.0F ? 1U : 0U};
}

}  // namespace neuralmesh
