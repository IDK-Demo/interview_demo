#include "byte_view_stream.h"

namespace idk::base {

ByteViewStream::ByteViewStream(ByteView byte_view)
    : read_it(0), byte_view(byte_view) {}

size_t ByteViewStream::size() const { return byte_view.size() - read_it; }

bool ByteViewStream::empty() const { return size() == 0; }


} // namespace eq::core
