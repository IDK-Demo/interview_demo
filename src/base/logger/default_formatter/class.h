#pragma once

#include "rfl/json.hpp"
#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"

#include "concept.h"
#include "base/logger/rfl_chrono_crutch/_chrono_parser.h"

template <idk::base::LoggableConcept T>
struct fmt::formatter<T> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const T& v, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", rfl::json::write(v));
  }
};

template <idk::base::LoggableConcept T>
struct fmtquill::formatter<T> : fmt::formatter<T> {};

template <idk::base::LoggableConcept T>
struct quill::Codec<T> : DirectFormatCodec<T>{};

template <idk::base::FastLoggableConcept T>
struct fmt::formatter<T> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const T& v, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", rfl::json::write(v));
  }
};

template <idk::base::FastLoggableConcept T>
struct fmtquill::formatter<T> : fmt::formatter<T> {};

template <idk::base::FastLoggableConcept T>
struct quill::Codec<T> : DeferredFormatCodec<T>{};

