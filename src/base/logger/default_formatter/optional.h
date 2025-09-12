#pragma once

#include <optional>

#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"

#include "fmt/format.h"
#include "rfl/json.hpp"

template <typename T>
struct fmt::formatter<std::optional<T>> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::optional<T>& p, FormatContext& ctx) const {
    if (p.has_value()) {
      return fmt::format_to(ctx.out(), "{}", p.value());
    }
    return fmt::format_to(ctx.out(), "");
  }
};

template <typename T>
struct fmtquill::formatter<std::optional<T>> : fmt::formatter<std::optional<T>> {};

template <typename T>
struct quill::Codec<std::optional<T>> : DeferredFormatCodec<std::optional<T>>{};
