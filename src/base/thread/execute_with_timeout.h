#pragma once
#include <atomic>

#include "base/clock/rdtsc_clock.h"
#include "thread.h"
#include "base/logger/macros.h"

namespace idk::base {

template <typename Func>
void execute_with_timeout(std::chrono::milliseconds timeout, Func&& func, const std::string& message) {
  std::atomic_flag finished;
  Thread thread([timeout, &message, &finished] {
    const auto start = RdtscClock::now();
    while (!finished.test() && RdtscClock::now() - start < timeout) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (!finished.test()) {
      ERROR("Timeout: {}", message);
      std::exit(1);
    }
  });
  try {
    func();
    finished.test_and_set();
  } catch (const std::exception& e) {
    finished.test_and_set();
    throw;
  }
}

}
