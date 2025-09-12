#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>


namespace idk::base {

template<class T>
T
load_unaligned(const void* ptr) {
  static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
  T value;
  std::memcpy(&value, ptr, sizeof(T));
  return value;
}

} // namespace idk::base
