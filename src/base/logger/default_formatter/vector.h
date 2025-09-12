#pragma once

#include <vector>

#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"

#include "fmt/format.h"
#include "rfl/json.hpp"

template <typename... Params>
struct fmt::formatter<std::vector<Params...>> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::vector<Params...>& v, FormatContext& ctx) const {
    if constexpr (std::same_as<typename std::vector<Params...>::value_type, const char*>) {
      std::stringstream ss;
      for (const char* s : v) {
        ss << s << ", ";
      }
      return fmt::format_to(ctx.out(), "[{}]", ss.str());
    } else {
      return fmt::format_to(ctx.out(), "{}", rfl::json::write(v));
    }
  }
};

template <typename... Params>
struct fmtquill::formatter<std::vector<Params...>> : fmt::formatter<std::vector<Params...>> {};

template <typename... Params>
struct quill::Codec<std::vector<Params...>> : DirectFormatCodec<std::vector<Params...>>{};
