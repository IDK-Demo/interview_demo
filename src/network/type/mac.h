#pragma once
#include <cstdint>
#include <string>
#include <netinet/in.h>
#include "base/logger/logger.h"

namespace idk::net {

class Mac {
public:
  static constexpr bool kLoggable = true;

  using ReflectionType = std::string;
  explicit Mac(const ReflectionType& str);
  Mac() = default;

  static constexpr size_t kSize = 6;

  [[nodiscard]] ReflectionType reflection() const;

  [[nodiscard]] auto
  data() const -> const uint8_t*;

  [[nodiscard]] auto
  data() -> uint8_t*;

  [[nodiscard]] auto
  as_string() const -> std::string;

  bool operator==(const Mac& other) const;
private:
  uint8_t mac_bytes[kSize]{};
};

}

template<>
struct fmt::formatter<idk::net::Mac> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto
  format(const idk::net::Mac& p, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", p.as_string());
  }
};

template<>
struct fmtquill::formatter<idk::net::Mac>
    : fmt::formatter<idk::net::Mac> {};

template<>
struct quill::Codec<idk::net::Mac>
    : DeferredFormatCodec<idk::net::Mac> {};