#include "test_helpers.hpp"

#include <memory>

#include "neuralmesh/buffer.hpp"

TEST_CASE(buffer_view_keeps_owner_alive) {
  auto owner = std::make_shared<neuralmesh::Buffer>(std::vector<unsigned char>{1, 2, 3, 4});
  neuralmesh::BufferView view(owner, 1, 2);
  owner.reset();

  REQUIRE(view.size() == 2);
  REQUIRE(view.data()[0] == 2);
}

TEST_CASE(little_endian_round_trip) {
  std::vector<unsigned char> bytes;
  neuralmesh::append_u32_le(bytes, 0x12345678);
  REQUIRE(neuralmesh::read_u32_le(bytes.data()) == 0x12345678);
}
