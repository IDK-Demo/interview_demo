#pragma once

#include <chrono>
#include <cinttypes>
#include <limits>
#include <ratio>
#include <thread>
#include <quill/BackendTscClock.h>

namespace idk::base {


class RdtscDuration {
public:
  using rep = uint64_t;

  constexpr RdtscDuration() noexcept : ticks_(0) {}

  explicit constexpr RdtscDuration(rep ticks) noexcept : ticks_(ticks) {}

  template <typename Rep, typename Period>
  RdtscDuration(const std::chrono::duration<Rep, Period>& d) noexcept;

  template <typename Rep, typename Period>
  operator std::chrono::duration<Rep, Period>() const noexcept;

  [[nodiscard]] constexpr rep count() const noexcept {
    return ticks_;
  }

  constexpr RdtscDuration operator+() const noexcept {
    return *this;
  }

  constexpr RdtscDuration operator-() const noexcept {
    return RdtscDuration(-ticks_);
  }

  RdtscDuration& operator++() noexcept {
    ++ticks_;
    return *this;
  }

  RdtscDuration operator++(int) noexcept {
    return RdtscDuration(ticks_++);
  }

  RdtscDuration& operator--() noexcept {
    --ticks_;
    return *this;
  }

  RdtscDuration operator--(int) noexcept {
    return RdtscDuration(ticks_--);
  }

  RdtscDuration& operator+=(const RdtscDuration& d) noexcept {
    ticks_ += d.ticks_;
    return *this;
  }

  RdtscDuration& operator-=(const RdtscDuration& d) noexcept {
    ticks_ -= d.ticks_;
    return *this;
  }

  RdtscDuration& operator*=(const rep& rhs) noexcept {
    ticks_ *= rhs;
    return *this;
  }

  RdtscDuration& operator/=(const rep& rhs) noexcept {
    ticks_ /= rhs;
    return *this;
  }

  RdtscDuration& operator%=(const rep& rhs) noexcept {
    ticks_ %= rhs;
    return *this;
  }

  RdtscDuration& operator%=(const RdtscDuration& rhs) noexcept {
    ticks_ %= rhs.ticks_;
    return *this;
  }

  // Zero value static function
  static constexpr RdtscDuration zero() noexcept {
    return RdtscDuration(0);
  }

  // Minimum value static function
  static constexpr RdtscDuration min() noexcept {
    return RdtscDuration(std::numeric_limits<rep>::lowest());
  }

  // Maximum value static function
  static constexpr RdtscDuration max() noexcept {
    return RdtscDuration(std::numeric_limits<rep>::max());
  }

  // Get the calibrated ticks per nanosecond value
  static double
  get_ticks_per_nanosecond() {
    // Initialize the thread_local optional value if not set
    if (!ticks_per_nanosecond) [[unlikely]] {
      const auto start_time = std::chrono::high_resolution_clock::now();
      const uint64_t start_ticks = quill::BackendTscClock::rdtsc().value();
      std::this_thread::sleep_for(std::chrono::milliseconds(kSampleTimeMs));
      const uint64_t end_ticks = quill::BackendTscClock::rdtsc().value();
      const auto end_time = std::chrono::high_resolution_clock::now();

      const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
      double ticks_per_ns = static_cast<double>(end_ticks - start_ticks) / elapsed_ns;
      ticks_per_nanosecond = ticks_per_ns;
    }
    return *ticks_per_nanosecond;
  }

private:
  static constexpr size_t kSampleTimeMs = 100;
  static thread_local std::optional<double> ticks_per_nanosecond;
  rep ticks_;
};

// Non-member arithmetic operators
constexpr RdtscDuration operator+(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return RdtscDuration(lhs.count() + rhs.count());
}

constexpr RdtscDuration operator-(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return RdtscDuration(lhs.count() - rhs.count());
}

constexpr RdtscDuration operator*(const RdtscDuration& lhs, const RdtscDuration::rep& rhs) noexcept {
  return RdtscDuration(lhs.count() * rhs);
}

constexpr RdtscDuration operator*(const RdtscDuration::rep& lhs, const RdtscDuration& rhs) noexcept {
  return RdtscDuration(lhs * rhs.count());
}

constexpr RdtscDuration operator/(const RdtscDuration& lhs, const RdtscDuration::rep& rhs) noexcept {
  return RdtscDuration(lhs.count() / rhs);
}

constexpr RdtscDuration::rep operator/(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() / rhs.count();
}

constexpr RdtscDuration operator%(const RdtscDuration& lhs, const RdtscDuration::rep& rhs) noexcept {
  return RdtscDuration(lhs.count() % rhs);
}

constexpr RdtscDuration operator%(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return RdtscDuration(lhs.count() % rhs.count());
}

constexpr bool operator==(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() == rhs.count();
}

constexpr bool operator!=(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() != rhs.count();
}

constexpr bool operator<(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() < rhs.count();
}

constexpr bool operator<=(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() <= rhs.count();
}

constexpr bool operator>(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() > rhs.count();
}

constexpr bool operator>=(const RdtscDuration& lhs, const RdtscDuration& rhs) noexcept {
  return lhs.count() >= rhs.count();
}

} // namespace idk::base