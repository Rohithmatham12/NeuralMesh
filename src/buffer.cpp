#include "neuralmesh/buffer.hpp"

#include <cstring>
#include <stdexcept>

namespace neuralmesh {

Buffer::Buffer(std::vector<std::uint8_t> bytes) : bytes_(std::move(bytes)) {}

const std::uint8_t* Buffer::data() const {
  return bytes_.data();
}

std::uint8_t* Buffer::data() {
  return bytes_.data();
}

std::size_t Buffer::size() const {
  return bytes_.size();
}

BufferView::BufferView(std::shared_ptr<const Buffer> owner, std::size_t offset, std::size_t length)
    : owner_(std::move(owner)), offset_(offset), length_(length) {
  if (!owner_ || offset_ + length_ > owner_->size()) {
    throw std::out_of_range("buffer view exceeds owner");
  }
}

const std::uint8_t* BufferView::data() const {
  return owner_->data() + offset_;
}

std::size_t BufferView::size() const {
  return length_;
}

std::shared_ptr<const Buffer> BufferView::owner() const {
  return owner_;
}

std::uint32_t read_u32_le(const std::uint8_t* data) {
  return static_cast<std::uint32_t>(data[0]) | (static_cast<std::uint32_t>(data[1]) << 8U) |
         (static_cast<std::uint32_t>(data[2]) << 16U) |
         (static_cast<std::uint32_t>(data[3]) << 24U);
}

float read_f32_le(const std::uint8_t* data) {
  const auto bits = read_u32_le(data);
  float value = 0.0F;
  static_assert(sizeof(value) == sizeof(bits), "float must be 32-bit");
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

void append_u32_le(std::vector<std::uint8_t>& out, std::uint32_t value) {
  out.push_back(static_cast<std::uint8_t>(value & 0xFFU));
  out.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xFFU));
  out.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xFFU));
  out.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xFFU));
}

void append_f32_le(std::vector<std::uint8_t>& out, float value) {
  std::uint32_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  append_u32_le(out, bits);
}

}  // namespace neuralmesh
