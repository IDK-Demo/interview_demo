#pragma once

#include <filesystem>
#include <format>
#include <fmt/format.h>

#include "quill/bundled/fmt/format.h"
#include "quill/DeferredFormatCodec.h"
#include "quill/DirectFormatCodec.h"

#include "fmt/format.h"
#include "rfl/json.hpp"

template <>
struct fmt::formatter<std::filesystem::path> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::filesystem::path& p, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", p.string());
  }
};

template <>
struct fmtquill::formatter<std::filesystem::path> : fmt::formatter<std::filesystem::path> {};

template <>
struct quill::Codec<std::filesystem::path> : DeferredFormatCodec<std::filesystem::path>{};
