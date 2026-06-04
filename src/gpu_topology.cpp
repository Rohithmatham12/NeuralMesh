#include "neuralmesh/gpu_topology.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace neuralmesh {

void GpuTopology::add_device(GpuDevice device) {
  devices_.push_back(std::move(device));
}

void GpuTopology::connect(std::string from, std::string to, LinkType link, double bandwidth_gbps) {
  links_.push_back({std::move(from), std::move(to), link, bandwidth_gbps});
}

TransferEstimate GpuTopology::estimate_transfer(const std::string& from, const std::string& to, std::uint64_t bytes) const {
  const auto found = std::find_if(links_.begin(), links_.end(), [&](const Link& link) {
    return (link.from == from && link.to == to) || (link.from == to && link.to == from);
  });
  if (found == links_.end()) {
    throw std::runtime_error("no GPU interconnect link found");
  }

  const double bytes_per_us = found->bandwidth_gbps * 1'000.0 / 8.0;
  return {found->type, bytes, found->bandwidth_gbps, static_cast<double>(bytes) / bytes_per_us};
}

const std::vector<GpuDevice>& GpuTopology::devices() const {
  return devices_;
}

std::string to_string(LinkType link) {
  return link == LinkType::NvLink ? "NVLink" : "PCIe";
}

}  // namespace neuralmesh
