#pragma once
#include <cstdint>
#include <string>
#include <netinet/in.h>

#include "base/logger/logger.h"

namespace idk::net {

class Ip {
public:
  static constexpr bool kLoggable = true;

  using ReflectionType = std::string;
  explicit Ip(const std::string& str);
  explicit Ip(uint32_t data);
  Ip() = default;

  [[nodiscard]] ReflectionType reflection() const;

  [[nodiscard]] auto
  data() const -> const uint32_t&;

  [[nodiscard]] auto
  data() -> uint32_t&;

  [[nodiscard]] auto
  as_string() const -> std::string;

  bool operator <(const Ip& rhs) const;

  bool operator==(const Ip& rhs) const;
private:
  in_addr addr{};
};

}

template<>
struct fmt::formatter<idk::net::Ip> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto
  format(const idk::net::Ip& p, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", p.as_string());
  }
};

template<>
struct fmtquill::formatter<idk::net::Ip>
    : fmt::formatter<idk::net::Ip> {};

template<>
struct quill::Codec<idk::net::Ip>
    : DeferredFormatCodec<idk::net::Ip> {};