#pragma once

#include <map>
#include <unordered_map>

#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"

#include "fmt/format.h"
#include "rfl/json.hpp"

template <typename... Params>
struct fmt::formatter<std::map<Params...>> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::map<Params...>& map, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", rfl::json::write(map));
  }
};

template <typename... Params>
struct fmtquill::formatter<std::map<Params...>> : fmt::formatter<std::map<Params...>> {};

template <typename... Params>
struct quill::Codec<std::map<Params...>> : DirectFormatCodec<std::map<Params...>>{};

template <typename... Params>
struct fmt::formatter<std::unordered_map<Params...>> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::unordered_map<Params...>& map, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", rfl::json::write(map));
  }
};

template <typename... Params>
struct fmtquill::formatter<std::unordered_map<Params...>> : fmt::formatter<std::unordered_map<Params...>> {};

template <typename... Params>
struct quill::Codec<std::unordered_map<Params...>> : DirectFormatCodec<std::unordered_map<Params...>>{};