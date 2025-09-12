#pragma once
#include "base/macros/require.h"
#include "base/type/span.h"
#include "base/type/start_lifetime_as.h"
#include "byte_view_stream.h"

namespace idk::base {

class Stream {
public:
  Stream(size_t buffer_size);

  template<typename Object>
  Object*
  peek() {
    if (sizeof(Object) > size()) {
      return nullptr;
    }
    return start_lifetime_as<Object>(&buffer[read]);
  }

  template<typename Object>
  Object*
  pop() {
    if (sizeof(Object) > size()) {
      return nullptr;
    }
    auto* ret = start_lifetime_as<Object>(&buffer[read]);
    pop(sizeof(Object));
    return ret;
  }

  std::optional<MutableByteView>
  pop(size_t n);

  std::optional<MutableByteView>
  peek(size_t n);

  std::optional<size_t>
  search(auto begin, auto end) {
    auto res = std::search(buffer.begin() + read, buffer.begin() + write, begin, end);
    if (res == buffer.begin() + write) {
      return {};
    }
    return res - buffer.begin() - read;
  }

  std::optional<size_t>
  search(StringView pattern) {
    return search(pattern.begin(), pattern.end());
  }

  uint8_t*
  data() {
    return buffer.data() + read;
  }

  size_t
  size() const;

  bool
  empty() const;

  void
  push_bytes(ByteView bytes);

  template<typename Object, typename... Args>
  Object&
  push(Args&&... args) {
    REQUIRE(write + sizeof(Object) <= buffer.size(), "Out of bounds");
    auto ptr = new (buffer.data() + write) Object(std::forward<Args>(args)...);
    write += sizeof(Object);
    return *ptr;
  }

  template<typename Object>
  Object&
  push(const Object& obj) {
    REQUIRE(write + sizeof(Object) <= buffer.size(), "Out of bounds");
    auto ptr = new (buffer.data() + write) Object(obj);
    write += sizeof(Object);
    return *ptr;
  }

  void
  push_bytes(const std::string& s) {
    REQUIRE(write + s.size() <= buffer.size(), "Out of bounds");
    std::ranges::copy(s, buffer.data() + write);
    write += s.size();
  }

  size_t
  available_suffix_bytes() const {
    return buffer.size() - write;
  }

  size_t
  total_available_bytes() const {
    return available_suffix_bytes() + read;
  }

  void
  shift() {
    std::copy_n(buffer.begin() + read, size(), buffer.begin());
    write = size();
    read = 0;
  }

  class ReadTransaction : protected ByteViewStream {
  public:
    ByteView
    commit() {
      return stream->pop(read_it).value();
    }

    using ByteViewStream::empty;
    using ByteViewStream::peek;
    using ByteViewStream::pop;
    using ByteViewStream::size;

  private:
    friend Stream;
    ReadTransaction(Stream* stream, ByteView byte_view) : ByteViewStream(byte_view), stream(stream) {}
    Stream* stream;
  };

  ReadTransaction read_transaction() {
    return ReadTransaction{this, peek(size()).value()};
  }

private:
  void
  shift_read(size_t n);

private:
  std::vector<uint8_t> buffer;
  size_t read;
  size_t write;
};

} // namespace idk::base
