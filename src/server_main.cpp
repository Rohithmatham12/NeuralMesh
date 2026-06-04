#include <iostream>

#include "neuralmesh/framed_rpc.hpp"
#include "neuralmesh/model.hpp"

int main() {
  neuralmesh::FramedRpcServer server(4);
  const neuralmesh::InferenceRequest request{1, {0.25F, 0.5F, -0.1F}};
  const auto response_frame = server.handle_frame(neuralmesh::frame_payload(neuralmesh::encode_request(request)));
  std::cout << "processed frame bytes=" << response_frame.size() << "\n";
  std::cout << server.event_log().to_json_lines();
  return 0;
}
