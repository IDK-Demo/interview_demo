#pragma once

#include <cstdint>
#include <cstring>
#include <concepts>
#include "quill/DeferredFormatCodec.h"

namespace idk::base {

/**
 * @brief A 24-bit unsigned integer type
 * 
 * This class represents a 24-bit unsigned integer, stored as 3 bytes.
 * It provides conversion to/from uint32_t and proper value semantics.
 */
class UInt24 {
public:
  using underlying_type = uint32_t;
  using ReflectionType = uint32_t;
  
  static constexpr uint32_t MAX_VALUE = 0xFFFFFF;
  
  // Default constructor - zero initialize
  constexpr UInt24() noexcept : bytes_{0, 0, 0} {}

  constexpr UInt24(uint32_t value) noexcept {
    set_value(value);
  }
  
  // Copy constructor
  constexpr UInt24(const UInt24&) noexcept = default;
  
  // Assignment operator from uint32_t
  constexpr UInt24& operator=(uint32_t value) noexcept {
    set_value(value);
    return *this;
  }
  
  // Copy assignment
  constexpr UInt24& operator=(const UInt24&) noexcept = default;
  
  // Conversion to uint32_t
  constexpr uint32_t value() const noexcept {
    // Little-endian storage
    return static_cast<uint32_t>(bytes_[2]) |
           (static_cast<uint32_t>(bytes_[1]) << 8) |
           (static_cast<uint32_t>(bytes_[0]) << 16);
  }

  [[nodiscard]] ReflectionType reflection() const {
    return value();
  }
  
  // Implicit conversion to uint32_t
  constexpr operator uint32_t() const noexcept {
    return value();
  }
  
  // Get raw bytes (for direct memory access)
  const uint8_t* data() const noexcept {
    return bytes_;
  }
  
  uint8_t* data() noexcept {
    return bytes_;
  }
  
  // Size in bytes
  static constexpr size_t size() noexcept {
    return 3;
  }
  
  // Comparison operators
  constexpr bool operator==(const UInt24& rhs) const noexcept {
    return bytes_[0] == rhs.bytes_[0] && 
           bytes_[1] == rhs.bytes_[1] && 
           bytes_[2] == rhs.bytes_[2];
  }
  
  constexpr bool operator!=(const UInt24& rhs) const noexcept {
    return !(*this == rhs);
  }
  
  constexpr bool operator<(const UInt24& rhs) const noexcept {
    return value() < rhs.value();
  }
  
  constexpr bool operator<=(const UInt24& rhs) const noexcept {
    return value() <= rhs.value();
  }
  
  constexpr bool operator>(const UInt24& rhs) const noexcept {
    return value() > rhs.value();
  }
  
  constexpr bool operator>=(const UInt24& rhs) const noexcept {
    return value() >= rhs.value();
  }
  
  // Arithmetic operators
  constexpr UInt24 operator+(const UInt24& rhs) const noexcept {
    return UInt24((value() + rhs.value()) & MAX_VALUE);
  }
  
  constexpr UInt24 operator-(const UInt24& rhs) const noexcept {
    return UInt24((value() - rhs.value()) & MAX_VALUE);
  }
  
  constexpr UInt24& operator+=(const UInt24& rhs) noexcept {
    set_value((value() + rhs.value()) & MAX_VALUE);
    return *this;
  }
  
  constexpr UInt24& operator-=(const UInt24& rhs) noexcept {
    set_value((value() - rhs.value()) & MAX_VALUE);
    return *this;
  }
  
  // Set value from big-endian bytes (network byte order)
  void set_from_big_endian(const uint8_t* bytes) noexcept {
    // Input is big-endian, store as little-endian
    bytes_[2] = bytes[2];
    bytes_[1] = bytes[1];
    bytes_[0] = bytes[0];
  }

private:
  constexpr void set_value(uint32_t value) noexcept {
    // Store in big-endian format
    bytes_[2] = value & 0xFF;
    bytes_[1] = (value >> 8) & 0xFF;
    bytes_[0] = (value >> 16) & 0xFF;
  }
  
  // Store as 3 bytes in little-endian order
  uint8_t bytes_[3];
} __attribute__((packed));

static_assert(sizeof(UInt24) == 3, "UInt24 must be exactly 3 bytes");
static_assert(std::is_standard_layout_v<UInt24>, "UInt24 must be standard layout");
static_assert(std::is_trivially_copyable_v<UInt24>, "UInt24 must be trivially copyable");

} // namespace idk::base

template<>
struct fmt::formatter<idk::base::UInt24> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto
  format(const idk::base::UInt24& data, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", data.value());
  }
};

IDK_LOGGER_REGISTER_ASYNC_TYPE(idk::base::UInt24);
