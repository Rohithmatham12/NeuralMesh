#include "test_helpers.hpp"

#include "neuralmesh/event_log.hpp"
#include "neuralmesh/framed_rpc.hpp"
#include "neuralmesh/model.hpp"

TEST_CASE(event_log_serializes_json_lines) {
  neuralmesh::EventLog log;
  log.append({42, "infer", 12.5});
  REQUIRE(log.records().size() == 1);
  REQUIRE(log.to_json_lines().find("\"request_id\":42") != std::string::npos);
}

TEST_CASE(framed_rpc_processes_inference_request) {
  neuralmesh::FramedRpcServer server(2);
  neuralmesh::InferenceRequest request{9, {1.0F, 1.0F}};
  auto response = server.handle_frame(neuralmesh::frame_payload(neuralmesh::encode_request(request)));

  REQUIRE(response.size() == 16);
  REQUIRE(server.event_log().records().size() == 1);
}
