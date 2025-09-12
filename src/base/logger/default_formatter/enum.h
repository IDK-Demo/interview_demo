#pragma once

#include "fmt/format.h"
#include "rfl.hpp"
#include "rfl/json.hpp"

#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"

#include "concept.h"

template <idk::base::ScopedEnumConcept Enum>
struct fmt::formatter<Enum> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Enum& v, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", rfl::enum_to_string(v));
  }
};

template <idk::base::ScopedEnumConcept T>
struct fmtquill::formatter<T> : fmt::formatter<T> {};

template <idk::base::ScopedEnumConcept T>
struct quill::Codec<T> : DeferredFormatCodec<T>{};

