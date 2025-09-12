#pragma once

#include <fstream>
#include "base/macros/require.h"
#include "base/type/span.h"
#include "base/type/start_lifetime_as.h"

namespace idk::base {

// Helpers to make a unique temp name
#define ASSIGN_OR_RETURN_CONCAT_INNER(x, y) x##y
#define ASSIGN_OR_RETURN_CONCAT(x, y) ASSIGN_OR_RETURN_CONCAT_INNER(x, y)

// Core implementation: works with pointer-like or optional-like results
#define ASSIGN_OR_RETURN_IMPL(tmp, lhs, rexpr, ret_value)                                                              \
  auto&& tmp = (rexpr);                                                                                                \
  if (!(tmp)) {                                                                                                        \
    return ret_value;                                                                                                  \
  }                                                                                                                    \
  lhs = *(tmp)

// Public macro
#define ASSIGN_OR_RETURN(lhs, rexpr, ret_value)                                                                                   \
  ASSIGN_OR_RETURN_IMPL(ASSIGN_OR_RETURN_CONCAT(_assign_tmp_, __COUNTER__), lhs, rexpr, ret_value)

class ByteViewStream {
public:
  explicit ByteViewStream(ByteView byte_view);

  template<typename Object>
  const Object*
  peek() const {
    if (size() < sizeof(Object)) {
      return nullptr;
    }
    return start_lifetime_as<Object>(byte_view.data() + read_it);
  }

  std::optional<ByteView>
  peek(size_t n) const {
    if (size() < n) {
      return {};
    }
    return ByteView{byte_view.data() + read_it, n};
  }

  template<typename Object>
  const Object*
  pop() {
    if (size() < sizeof(Object)) {
      return nullptr;
    }
    auto* ret = start_lifetime_as<Object>(byte_view.data() + read_it);
    pop(sizeof(Object));
    return ret;
  }

  std::optional<ByteView>
  pop(size_t n) {
    if (size() < n) {
      return {};
    }
    auto ret = ByteView{byte_view.data() + read_it, n};
    read_it += n;
    return ret;
  }

  [[nodiscard]] size_t
  size() const;

  [[nodiscard]] bool
  empty() const;

protected:
  size_t read_it;
  ByteView byte_view;
};

} // namespace idk::base
