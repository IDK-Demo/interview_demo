#pragma once

#include <cstddef>

namespace idk::base {

namespace impl {
  template<size_t kI, typename F>
  void recursive(F&& f) {
    if constexpr (kI > 0) {
      recursive<kI - 1>(f);
    }
    f.template operator()<kI>();
  }
}

template<size_t kSize, typename F>
void constexpr_for(F&& f) {
  if constexpr (kSize > 0) {
    impl::recursive<kSize - 1>(f);
  }
}

}
