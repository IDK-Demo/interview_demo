#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include "base/logger/logger.h"

namespace idk::base {

using MutableStringView = std::span<char>;
using StringView = std::string_view;

using ByteView = std::span<const uint8_t>;
using MutableByteView = std::span<uint8_t>;

template <typename T>
ByteView
to_byte_view(const T& obj) {
  return {reinterpret_cast<const uint8_t*>(&obj), sizeof(T)};
}

template <typename T>
MutableByteView
to_mutable_byte_view(T& obj) {
  return {reinterpret_cast<uint8_t*>(&obj), sizeof(T)};
}

inline StringView
to_string_view(ByteView bytes) {
  return {reinterpret_cast<const char*>(bytes.data()), bytes.size()};
}

inline StringView
to_string_view(MutableByteView bytes) {
  return {reinterpret_cast<char*>(bytes.data()), bytes.size()};
}

template<typename T>
using ArrayView = std::span<const T>;

namespace impl {

template<typename T>
concept ByteViewConcept = std::same_as<T, ByteView> || std::same_as<T, MutableByteView>;

}

} // namespace idk::base


template<idk::base::impl::ByteViewConcept T>
struct fmt::formatter<T> {
  // Delimiter between bytes (default ' ')
  char delimiter = ',';
  // parse format specifier (we only support a single optional char as delimiter)
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    auto it = ctx.begin();
    auto end = ctx.end();
    // if user writes e.g. "{:,-}", use ',' as delimiter
    if (it != end && *it != '}') {
      delimiter = *it++;
    }
    if (it != end && *it != '}')
      throw fmt::format_error("invalid format for base::MutableByteView");
    return it;
  }

  template<typename FormatContext>
  auto
  format(T data, FormatContext& ctx) const {
    auto out = ctx.out();
    bool first = true;
    out = fmt::format_to(out, "[");
    for (uint8_t byte: data) {
      if (!first)
        *out++ = delimiter;
      first = false;
      out = fmt::format_to(out, "{:02X}", byte);
    }
    out = fmt::format_to(out, "]");
    return out;
  }
};

IDK_LOGGER_REGISTER_SYNC_CONCEPT(idk::base::impl::ByteViewConcept);
