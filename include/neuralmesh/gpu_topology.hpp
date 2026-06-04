#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace neuralmesh {

enum class LinkType {
  Pcie,
  NvLink
};

struct GpuDevice {
  std::string id;
  std::uint64_t hbm_bytes{0};
  double hbm_bandwidth_gbps{0.0};
  int numa_node{0};
};

struct TransferEstimate {
  LinkType link{LinkType::Pcie};
  std::uint64_t bytes{0};
  double bandwidth_gbps{0.0};
  double latency_us{0.0};
};

class GpuTopology {
 public:
  void add_device(GpuDevice device);
  void connect(std::string from, std::string to, LinkType link, double bandwidth_gbps);
  TransferEstimate estimate_transfer(const std::string& from, const std::string& to, std::uint64_t bytes) const;
  const std::vector<GpuDevice>& devices() const;

 private:
  struct Link {
    std::string from;
    std::string to;
    LinkType type;
    double bandwidth_gbps;
  };

  std::vector<GpuDevice> devices_;
  std::vector<Link> links_;
};

std::string to_string(LinkType link);

}  // namespace neuralmesh
