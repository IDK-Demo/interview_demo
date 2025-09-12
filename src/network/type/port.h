#pragma once
#include <bit>
#include "big_endian.h"

namespace idk::net {

class Port {
public:
  constexpr Port() = default;

  explicit constexpr Port(uint16_t port) : data_(port) {}

  using ReflectionType = uint16_t;

  ReflectionType
  reflection() const {
    return value();
  }

  [[nodiscard]] constexpr uint16_t
  value() const {
    return data_.value();
  }

  [[nodiscard]] constexpr uint16_t
  as_big_endian() const {
    return data_.data();
  }

  constexpr auto
  operator==(const Port& rhs) const {
    return data_ == rhs.data_;
  };

private:
  BE<uint16_t> data_;
};

} // namespace idk::net

template<>
struct fmt::formatter<idk::net::Port> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto
  format(const idk::net::Port& p, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", p.value());
  }
};

template<>
struct fmtquill::formatter<idk::net::Port> : fmt::formatter<idk::net::Port> {};

template<>
struct quill::Codec<idk::net::Port> : DeferredFormatCodec<idk::net::Port> {};
