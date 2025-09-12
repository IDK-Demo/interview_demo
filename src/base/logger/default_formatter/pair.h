#pragma once

#include <tuple>
#include <fmt/format.h>

#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"

#include "fmt/format.h"
#include "rfl/json.hpp"

template <typename T1, typename T2>
struct fmt::formatter<std::pair<T1, T2>> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::pair<T1, T2>& p, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "({}, {})", p.first, p.second);
  }
};


template <typename T1, typename T2>
struct fmtquill::formatter<std::pair<T1, T2>> : fmt::formatter<std::pair<T1, T2>> {};

template <typename T1, typename T2>
struct quill::Codec<std::pair<T1, T2>> : DirectFormatCodec<std::pair<T1, T2>>{};
