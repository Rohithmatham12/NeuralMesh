#include "neuralmesh/cluster.hpp"

#include <algorithm>
#include <functional>
#include <stdexcept>

namespace neuralmesh {
namespace {

std::uint64_t stable_hash(const std::string& value) {
  return static_cast<std::uint64_t>(std::hash<std::string>{}(value));
}

}  // namespace

void ConsistentHashRouter::add_node(GpuNode node) {
  nodes_.push_back(std::move(node));
  rebuild_ring();
}

void ConsistentHashRouter::set_health(const std::string& node_id, bool healthy) {
  for (auto& node : nodes_) {
    if (node.id == node_id) {
      node.healthy = healthy;
      return;
    }
  }
  throw std::runtime_error("unknown node: " + node_id);
}

std::optional<GpuNode> ConsistentHashRouter::route(const std::string& key) const {
  if (ring_.empty()) {
    return std::nullopt;
  }
  const auto key_hash = stable_hash(key);
  auto it = std::lower_bound(ring_.begin(), ring_.end(), key_hash, [](const RingEntry& entry, std::uint64_t hash) {
    return entry.hash < hash;
  });
  if (it == ring_.end()) {
    it = ring_.begin();
  }

  for (std::size_t attempts = 0; attempts < ring_.size(); ++attempts) {
    const auto& candidate = nodes_[it->node_index];
    if (candidate.healthy) {
      return candidate;
    }
    ++it;
    if (it == ring_.end()) {
      it = ring_.begin();
    }
  }
  return std::nullopt;
}

std::vector<std::string> ConsistentHashRouter::ring_order() const {
  std::vector<std::string> order;
  order.reserve(ring_.size());
  for (const auto& entry : ring_) {
    order.push_back(nodes_[entry.node_index].id);
  }
  return order;
}

std::size_t ConsistentHashRouter::node_count() const {
  return nodes_.size();
}

void ConsistentHashRouter::rebuild_ring() {
  ring_.clear();
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    for (std::uint32_t replica = 0; replica < nodes_[i].virtual_nodes; ++replica) {
      ring_.push_back({stable_hash(nodes_[i].id + "#" + std::to_string(replica)), i});
    }
  }
  std::sort(ring_.begin(), ring_.end(), [](const RingEntry& lhs, const RingEntry& rhs) {
    return lhs.hash < rhs.hash;
  });
}

FailoverResult inject_failure_and_route(ConsistentHashRouter& router, const std::string& failed_node, const std::string& key) {
  const auto before = router.route(key);
  router.set_health(failed_node, false);
  const auto after = router.route(key);
  if (!after) {
    throw std::runtime_error("no healthy failover target");
  }
  return {before ? before->id : "", after->id, before && before->id != after->id};
}

}  // namespace neuralmesh
