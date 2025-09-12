#pragma once


#include "rdtsc_clock/duration.h"
#include "rdtsc_clock/time_point.h"
#include "quill/BackendTscClock.h"

namespace idk::base {

class RdtscClock {
public:
  using duration = RdtscDuration;
  using time_point = RdtscTimePoint<RdtscClock, RdtscDuration>;
  static constexpr bool is_steady = true;

  static time_point
  now() noexcept {
    return time_point(duration(quill::BackendTscClock::rdtsc().value()));
  }
};

} // namespace idk::base
