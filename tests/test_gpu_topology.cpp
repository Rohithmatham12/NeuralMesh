#include "test_helpers.hpp"

#include "neuralmesh/gpu_topology.hpp"

TEST_CASE(gpu_topology_estimates_nvlink_transfer_latency) {
  neuralmesh::GpuTopology topology;
  topology.add_device({"gpu0", 80ULL << 30U, 3000.0, 0});
  topology.add_device({"gpu1", 80ULL << 30U, 3000.0, 0});
  topology.connect("gpu0", "gpu1", neuralmesh::LinkType::NvLink, 600.0);

  const auto estimate = topology.estimate_transfer("gpu0", "gpu1", 64ULL << 20U);
  REQUIRE(estimate.link == neuralmesh::LinkType::NvLink);
  REQUIRE(estimate.latency_us > 0.0);
  REQUIRE(neuralmesh::to_string(estimate.link) == "NVLink");
}
