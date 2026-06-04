#include "test_helpers.hpp"

#include "neuralmesh/cluster.hpp"

TEST_CASE(consistent_hash_routes_to_healthy_gpu_node) {
  neuralmesh::ConsistentHashRouter router;
  router.add_node({"gpu-a", "10.0.0.1:50051", 8});
  router.add_node({"gpu-b", "10.0.0.2:50051", 8});

  const auto node = router.route("tenant-a:model-x");
  REQUIRE(node.has_value());
  REQUIRE(node->healthy);
}

TEST_CASE(fault_injection_fails_over_from_unhealthy_node) {
  neuralmesh::ConsistentHashRouter router;
  router.add_node({"gpu-a", "10.0.0.1:50051", 64});
  router.add_node({"gpu-b", "10.0.0.2:50051", 64});

  const auto first = router.route("fixed-key");
  REQUIRE(first.has_value());
  const auto result = neuralmesh::inject_failure_and_route(router, first->id, "fixed-key");
  REQUIRE(result.selected != first->id);
  REQUIRE(result.failed_over);
}

TEST_CASE(node_stats_are_cache_line_aligned) {
  REQUIRE(alignof(neuralmesh::NodeStats) == 64);
  REQUIRE(sizeof(neuralmesh::NodeStats) == 64);
}
