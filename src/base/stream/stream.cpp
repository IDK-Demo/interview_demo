#include "stream.h"
#include <algorithm>
#include <ranges>
#include "base/macros/require.h"
#include "base/type/start_lifetime_as.h"

namespace idk::base {

Stream::Stream(size_t buffer_size) : buffer(buffer_size), read(0), write(0) {}

std::optional<MutableByteView>
Stream::pop(size_t n) {
  if (n > size()) {
    return {};
  }
  auto res = peek(n);
  shift_read(n);
  return res;
}

std::optional<MutableByteView>
Stream::peek(size_t n) {
  if (n > size()) {
    return {};
  }
  return MutableByteView{&buffer[read], n};
}

size_t
Stream::size() const {
  return write - read;
}

bool
Stream::empty() const {
  return size() == 0;
}

void
Stream::push_bytes(ByteView bytes) {
  REQUIRE(available_suffix_bytes() >= bytes.size(), "Not enough space");
  std::ranges::copy(bytes, buffer.begin() + write);
  write += bytes.size();
}

void
Stream::shift_read(size_t n) {
  REQUIRE(n <= size(), "Not enough bytes");
  read += n;
  if (read == write) {
    read = 0;
    write = 0;
  }
}

} // namespace idk::base
