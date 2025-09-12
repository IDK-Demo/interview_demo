#pragma once

#include <chrono>
#include "quill/BackendTscClock.h"

#include "base/logger/logger.h"
#include "rdtsc_clock.h"


namespace idk::base {

class SyncRdtscClock {
public:
  using duration = std::chrono::nanoseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<quill::BackendTscClock, duration>;
  static constexpr bool is_steady = false;

  static time_point
  now() noexcept {
    return quill::BackendTscClock::now();
  }

  static time_point
  from_rdtsc(RdtscClock::time_point rdtsc) noexcept {
    uint64_t ticks = rdtsc.time_since_epoch().count();
    auto& rdtsc_val = reinterpret_cast<quill::BackendTscClock::RdtscVal&>(ticks);
    return quill::BackendTscClock::to_time_point(rdtsc_val);
  }
};

} // namespace idk::base


template <>
struct fmt::formatter<idk::base::SyncRdtscClock::time_point> {
  template<typename TFormatParseCtx>
  constexpr auto
  parse(TFormatParseCtx& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const idk::base::SyncRdtscClock::time_point& p, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", p.time_since_epoch().count());
  }
};

template <>
struct fmtquill::formatter<idk::base::SyncRdtscClock::time_point> : fmt::formatter<idk::base::SyncRdtscClock::time_point> {};

template <>
struct quill::Codec<idk::base::SyncRdtscClock::time_point> : DeferredFormatCodec<idk::base::SyncRdtscClock::time_point>{};
