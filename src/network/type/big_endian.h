#pragma once
#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>

#include "base/logger/logger.h"

namespace idk::net {

// Concept for types that have underlying_type and value() method
template<typename T>
concept HasUnderlyingTypeAndValue = requires(const T& t) {
  typename T::underlying_type;
  { t.value() } -> std::convertible_to<typename T::underlying_type>;
  requires std::is_integral_v<typename T::underlying_type>;
};

namespace detail {
template<typename T>
struct underlying_type_helper {
  using type = T;
};

template<typename T>
  requires std::is_enum_v<T>
struct underlying_type_helper<T> {
  using type = std::underlying_type_t<T>;
};

template<typename T>
  requires HasUnderlyingTypeAndValue<T>
struct underlying_type_helper<T> {
  using type = typename T::underlying_type;
};
} // namespace detail

template<typename T>
class BE {
  static_assert(std::endian::native == std::endian::little);
  static_assert(std::is_integral_v<T> || std::is_enum_v<T> || HasUnderlyingTypeAndValue<T>, 
                "Type must be integral, enum, or have underlying_type and value() method");

public:
  using value_type = T;
  using underlying_type = detail::underlying_type_helper<T>::type;

  static constexpr bool kLoggable = true;
  using ReflectionType = value_type;

  constexpr BE() = default;
  constexpr BE(T value) : value_(std::byteswap(extract_underlying_value(value))) {}

  BE&
  operator=(const T& b) {
    value_ = std::byteswap(extract_underlying_value(b));
    return *this;
  }

  ReflectionType
  reflection() const {
    return value();
  }

  static constexpr BE
  from_big_endian(T value) noexcept {
    return BE(extract_underlying_value(value), nullptr);
  }

  constexpr value_type
  value() const noexcept {
    if constexpr (HasUnderlyingTypeAndValue<T>) {
      return T(std::byteswap(value_));
    } else if constexpr (std::is_enum_v<T>) {
      return static_cast<T>(std::byteswap(value_));
    } else {
      return std::byteswap(value_);
    }
  }

  constexpr underlying_type
  data() const noexcept {
    return value_;
  }

  bool
  operator==(const BE& rhs) const noexcept {
    return value_ == rhs.value_;
  }

private:
  static constexpr underlying_type extract_underlying_value(const T& value) {
    if constexpr (HasUnderlyingTypeAndValue<T>) {
      return value.value();
    } else {
      return static_cast<underlying_type>(value);
    }
  }

  struct BigEndianTag;
  constexpr BE(underlying_type value, BigEndianTag*) : value_(value) {};
  underlying_type value_{};
};


} // namespace idk::base

template<typename T>
struct fmtquill::formatter<idk::net::BE<T>> : fmt::formatter<idk::net::BE<T>> {};

template<typename T>
struct quill::Codec<idk::net::BE<T>> : DeferredFormatCodec<idk::net::BE<T>> {};

