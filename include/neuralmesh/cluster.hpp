#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace neuralmesh {

struct alignas(64) NodeStats {
  std::uint64_t requests{0};
  std::uint64_t failures{0};
  char padding[64 - 2 * sizeof(std::uint64_t)]{};
};

struct GpuNode {
  std::string id;
  std::string endpoint;
  std::uint32_t virtual_nodes{32};
  bool healthy{true};
  NodeStats stats;
};

class ConsistentHashRouter {
 public:
  void add_node(GpuNode node);
  void set_health(const std::string& node_id, bool healthy);
  std::optional<GpuNode> route(const std::string& key) const;
  std::vector<std::string> ring_order() const;
  std::size_t node_count() const;

 private:
  struct RingEntry {
    std::uint64_t hash;
    std::size_t node_index;
  };

  std::vector<GpuNode> nodes_;
  std::vector<RingEntry> ring_;

  void rebuild_ring();
};

struct FailoverResult {
  std::string primary;
  std::string selected;
  bool failed_over{false};
};

FailoverResult inject_failure_and_route(ConsistentHashRouter& router, const std::string& failed_node, const std::string& key);

}  // namespace neuralmesh
