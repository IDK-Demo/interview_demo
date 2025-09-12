#pragma once

#include <cstring>
#include <fstream>
#include "base/macros/require.h"
#include "base/type/start_lifetime_as.h"
#include "base/type/span.h"

namespace idk::base {

class MutableByteViewWriteStream {
public:
  explicit MutableByteViewWriteStream(MutableByteView byte_view) : write_it(0), byte_view(byte_view) {}

  template <typename Object, typename... Args>
  Object& push(Args&&... args) {
    REQUIRE(write_it + sizeof(Object) <= byte_view.size(), "Out of bounds");
    auto ptr = new(byte_view.data() + write_it) Object(std::forward<Args>(args)...);
    write_it += sizeof(Object);
    return *ptr;
  }

  template <typename Object>
  Object& push(const Object& obj) {
    REQUIRE(write_it + sizeof(Object) <= byte_view.size(), "Out of bounds");
    auto ptr = new(byte_view.data() + write_it) Object(obj);
    write_it += sizeof(Object);
    return *ptr;
  }

  void push_bytes(ByteView bytes) {
    REQUIRE(write_it + bytes.size() <= byte_view.size(), "Out of bounds");
    std::ranges::copy(bytes, byte_view.data() + write_it);
    write_it += bytes.size();
  }

  void push(const std::string& s) {
    REQUIRE(write_it + s.size() <= byte_view.size(), "Out of bounds");
    std::ranges::copy(s, byte_view.data() + write_it);
    write_it += s.size();
  }

  MutableByteView reserve(size_t n) {
    REQUIRE(write_it + n <= byte_view.size(), "Out of bounds");
    MutableByteView res(byte_view.data() + write_it, n);
    write_it += n;
    return res;
  }

  [[nodiscard]] size_t size() const {
    return write_it;
  }

  [[nodiscard]] uint8_t* data() const {
    return byte_view.data();
  }

  [[nodiscard]] bool empty() const {
    return size() == 0;
  }

private:
  size_t write_it;
  MutableByteView byte_view;
};

} // namespace eq::core
