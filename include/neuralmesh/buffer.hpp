#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace neuralmesh {

class Buffer {
 public:
  explicit Buffer(std::vector<std::uint8_t> bytes);

  const std::uint8_t* data() const;
  std::uint8_t* data();
  std::size_t size() const;

 private:
  std::vector<std::uint8_t> bytes_;
};

class BufferView {
 public:
  BufferView(std::shared_ptr<const Buffer> owner, std::size_t offset, std::size_t length);

  const std::uint8_t* data() const;
  std::size_t size() const;
  std::shared_ptr<const Buffer> owner() const;

 private:
  std::shared_ptr<const Buffer> owner_;
  std::size_t offset_;
  std::size_t length_;
};

std::uint32_t read_u32_le(const std::uint8_t* data);
float read_f32_le(const std::uint8_t* data);
void append_u32_le(std::vector<std::uint8_t>& out, std::uint32_t value);
void append_f32_le(std::vector<std::uint8_t>& out, float value);

}  // namespace neuralmesh
