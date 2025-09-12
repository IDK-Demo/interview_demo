#pragma once

#include <compare>
#include <cstdint>

namespace idk::net::tcp {

class SeqNumber {
public:
  using underlying_type = uint32_t;
  static constexpr bool kLoggable = true;
  using ReflectionType = underlying_type;

  constexpr SeqNumber() noexcept : value_(0) {}

  constexpr SeqNumber(uint32_t value) noexcept : value_(value) {}

  ReflectionType
  reflection() const {
    return value();
  }

  [[nodiscard]] constexpr uint32_t
  value() const noexcept {
    return value_;
  }

  explicit constexpr
  operator uint32_t() const noexcept {
    return value_;
  }

  [[nodiscard]] constexpr SeqNumber
  operator+(uint32_t offset) const noexcept {
    return SeqNumber(value_ + offset); // Automatic wrap-around due to uint32_t overflow
  }

  constexpr SeqNumber&
  operator+=(uint32_t offset) noexcept {
    value_ += offset; // Automatic wrap-around due to uint32_t overflow
    return *this;
  }

  [[nodiscard]] constexpr uint32_t
  operator-(const SeqNumber& other) const noexcept {
    return value_ - other.value_; // Automatic wrap-around due to uint32_t underflow
  }

  [[nodiscard]] constexpr std::strong_ordering
  operator<=>(const SeqNumber& other) const noexcept {
    if (value_ == other.value_) {
      return std::strong_ordering::equal;
    }

    if (static_cast<int32_t>(value_ - other.value_) < 0) {
      return std::strong_ordering::less;
    }
    return std::strong_ordering::greater;
  }

  [[nodiscard]] constexpr bool
  operator==(const SeqNumber& other) const noexcept {
    return value_ == other.value_;
  }

private:
  uint32_t value_;
};

} // namespace idk::net
