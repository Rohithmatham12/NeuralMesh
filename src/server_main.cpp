#include <iostream>

#include "neuralmesh/framed_rpc.hpp"
#include "neuralmesh/cluster.hpp"
#include "neuralmesh/gpu_topology.hpp"
#include "neuralmesh/model.hpp"

int main() {
  neuralmesh::FramedRpcServer server(4);
  const neuralmesh::InferenceRequest request{1, {0.25F, 0.5F, -0.1F}};
  const auto response_frame = server.handle_frame(neuralmesh::frame_payload(neuralmesh::encode_request(request)));
  std::cout << "processed frame bytes=" << response_frame.size() << "\n";
  std::cout << server.event_log().to_json_lines();

  neuralmesh::ConsistentHashRouter router;
  router.add_node({"gpu-a", "10.0.0.1:50051", 16});
  router.add_node({"gpu-b", "10.0.0.2:50051", 16});
  const auto routed = router.route("tenant-a:model-x");
  std::cout << "routed_node=" << (routed ? routed->id : "none") << "\n";

  neuralmesh::GpuTopology topology;
  topology.add_device({"gpu0", 80ULL << 30U, 3000.0, 0});
  topology.add_device({"gpu1", 80ULL << 30U, 3000.0, 0});
  topology.connect("gpu0", "gpu1", neuralmesh::LinkType::NvLink, 600.0);
  const auto estimate = topology.estimate_transfer("gpu0", "gpu1", 64ULL << 20U);
  std::cout << "transfer=" << neuralmesh::to_string(estimate.link) << " latency_us=" << estimate.latency_us << "\n";
  return 0;
}
