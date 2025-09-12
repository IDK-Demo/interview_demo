#pragma once

#include <chrono>
#include <cinttypes>
#include <limits>

namespace idk::base {


template<typename ClockT, typename DurationT>
class RdtscTimePoint {
public:
  using duration = DurationT;
  using clock = ClockT;

  constexpr RdtscTimePoint() noexcept : ticks_(0) {}

  constexpr explicit RdtscTimePoint(const duration& d) noexcept
    : ticks_(d.count()) {}

  template <typename Clock, typename Duration>
  RdtscTimePoint(const std::chrono::time_point<Clock, Duration>& tp) noexcept {
    const auto d = tp.time_since_epoch();
    duration tick_duration(d);
    ticks_ = tick_duration.count();
  }

  template <typename Clock, typename Duration>
  operator std::chrono::time_point<Clock, Duration>() const noexcept {
    duration d(ticks_);
    Duration std_duration = d;
    return std::chrono::time_point<Clock, Duration>(std_duration);
  }

  constexpr duration time_since_epoch() const noexcept {
    return ticks_;
  }
  
  RdtscTimePoint& operator+=(const duration& d) noexcept {
    ticks_ += d;
    return *this;
  }

  RdtscTimePoint& operator-=(const duration& d) noexcept {
    ticks_ -= d;
    return *this;
  }

  static constexpr RdtscTimePoint min() noexcept {
    return TimePoint(duration::min());
  }
  
  static constexpr RdtscTimePoint max() noexcept {
    return TimePoint(duration::max());
  }
  
private:
  duration ticks_;
};

template<typename C, typename D, typename T>
constexpr RdtscTimePoint<C, D> operator+(const RdtscTimePoint<C, D>& lhs, const T& rhs) noexcept {
  return RdtscTimePoint<C, D>(lhs.time_since_epoch() + rhs);
}

template<typename C, typename D, typename T>
constexpr RdtscTimePoint<C, D> operator+(const T& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return RdtscTimePoint<C, D>(lhs + rhs.time_since_epoch());
}

template<typename C, typename D, typename T>
constexpr RdtscTimePoint<C, D> operator-(const RdtscTimePoint<C, D>& lhs, const T& rhs) noexcept {
  return RdtscTimePoint<C, D>(lhs.time_since_epoch() - rhs);
}

template<typename C, typename D>
constexpr D operator-(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() - rhs.time_since_epoch();
}

template<typename C, typename D>
constexpr bool operator==(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() == rhs.time_since_epoch();
}

template<typename C, typename D>
constexpr bool operator!=(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() != rhs.time_since_epoch();
}

template<typename C, typename D>
constexpr bool operator<(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() < rhs.time_since_epoch();
}

template<typename C, typename D>
constexpr bool operator<=(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() <= rhs.time_since_epoch();
}

template<typename C, typename D>
constexpr bool operator>(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() > rhs.time_since_epoch();
}

template<typename C, typename D>
constexpr bool operator>=(const RdtscTimePoint<C, D>& lhs, const RdtscTimePoint<C, D>& rhs) noexcept {
  return lhs.time_since_epoch() >= rhs.time_since_epoch();
}

} // namespace idk::base