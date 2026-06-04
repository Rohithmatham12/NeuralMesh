#pragma once

#include <cstdint>
#include <vector>

#include "neuralmesh/buffer.hpp"

namespace neuralmesh {

struct InferenceRequest {
  std::uint32_t request_id{0};
  std::vector<float> features;
};

struct InferenceResponse {
  std::uint32_t request_id{0};
  float score{0.0F};
  std::uint32_t label{0};
};

InferenceRequest parse_request(const BufferView& view);
std::vector<std::uint8_t> encode_request(const InferenceRequest& request);
std::vector<std::uint8_t> encode_response(const InferenceResponse& response);
InferenceResponse run_model(const InferenceRequest& request);

}  // namespace neuralmesh
