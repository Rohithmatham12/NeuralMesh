#include "test_helpers.hpp"

#include <memory>

#include "neuralmesh/model.hpp"

TEST_CASE(model_request_round_trip_and_score) {
  neuralmesh::InferenceRequest request{7, {1.0F, 2.0F, -1.0F}};
  auto bytes = neuralmesh::encode_request(request);
  auto buffer = std::make_shared<neuralmesh::Buffer>(bytes);
  auto parsed = neuralmesh::parse_request({buffer, 0, buffer->size()});
  auto response = neuralmesh::run_model(parsed);

  REQUIRE(parsed.request_id == 7);
  REQUIRE(parsed.features.size() == 3);
  REQUIRE(response.score == 2.0F);
  REQUIRE(response.label == 1);
}
