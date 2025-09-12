#include "duration.h"

namespace idk::base {

thread_local std::optional<double> RdtscDuration::ticks_per_nanosecond{};

template <typename Rep, typename Period>
RdtscDuration::RdtscDuration(const std::chrono::duration<Rep, Period>& d) noexcept {
  const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
  ticks_ = static_cast<rep>(ns * get_ticks_per_nanosecond());
}

template <typename Rep, typename Period>
RdtscDuration::operator std::chrono::duration<Rep, Period>() const noexcept {
  const auto ns = static_cast<double>(ticks_) / get_ticks_per_nanosecond();
  return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
      std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(ns)));
}

template RdtscDuration::RdtscDuration(const std::chrono::nanoseconds&) noexcept;
template RdtscDuration::RdtscDuration(const std::chrono::microseconds&) noexcept;
template RdtscDuration::RdtscDuration(const std::chrono::milliseconds&) noexcept;
template RdtscDuration::RdtscDuration(const std::chrono::seconds&) noexcept;
template RdtscDuration::RdtscDuration(const std::chrono::minutes&) noexcept;
template RdtscDuration::RdtscDuration(const std::chrono::hours&) noexcept;

template RdtscDuration::operator std::chrono::nanoseconds() const noexcept;
template RdtscDuration::operator std::chrono::microseconds() const noexcept;
template RdtscDuration::operator std::chrono::milliseconds() const noexcept;
template RdtscDuration::operator std::chrono::seconds() const noexcept;
template RdtscDuration::operator std::chrono::minutes() const noexcept;
template RdtscDuration::operator std::chrono::hours() const noexcept;

} // namespace idk::base