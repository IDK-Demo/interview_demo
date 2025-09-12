#pragma once

#include <cstdint>
#include <cstring>
#include <optional>

#include "base/stream/byte_view_stream.h"
#include "network/type/big_endian.h"

#include "base/stream/stream.h"

namespace idk::net::wss {

enum class OpCode : uint8_t {
  Continuation = 0x0,
  Text = 0x1,
  Binary = 0x2,
  Close = 0x8,
  Ping = 0x9,
  Pong = 0xA,
};

#pragma pack(push, 1)

enum class FrameType : uint8_t {
  Base = 0,
  Extended16,
  Extended64
};

struct FrameBasicHeader {
  uint8_t code : 4;
  bool rsv3 : 1;
  bool rsv2 : 1;
  bool rsv1 : 1;
  bool fin : 1;

  uint8_t payload_len : 7;
  bool mask : 1;

  FrameType type() const {
    if (payload_len < 125) {
      return FrameType::Base;
    }
    if (payload_len == 126) {
      return FrameType::Extended16;
    }
    return FrameType::Extended64;
  }

  size_t payload_size() const {
    return payload_len;
  }
};

struct FrameExtended16 : FrameBasicHeader {
  BE<uint16_t> extended_payload_len;

  size_t payload_size() const {
    return extended_payload_len.value();
  }
};

struct FrameExtended64 : FrameBasicHeader {
  BE<uint64_t> extended_payload_len;

  size_t payload_size() const {
    return extended_payload_len.value();
  }
};

struct FrameMask {
  uint8_t mask_key[4];
};

#pragma pack(pop)

struct FrameHeader {
  static constexpr bool kLoggable = true;
  OpCode code;
  bool rsv3;
  bool rsv2;
  bool rsv1;
  bool fin;
  bool mask;
  size_t size;
  size_t header_size;
};

template<typename HeaderType>
std::optional<FrameHeader>
pop_header_impl(base::Stream& stream) {
  ASSIGN_OR_RETURN(auto& header, stream.pop<HeaderType>(), {});
  return FrameHeader{
    .code = OpCode(header.code),
    .rsv3 = header.rsv3,
    .rsv2 = header.rsv2,
    .rsv1 = header.rsv1,
    .fin = header.fin,
    .mask = header.mask,
    .size = header.payload_size(),
    .header_size = sizeof(HeaderType)
  };
}

inline std::optional<FrameHeader>
pop_header(base::Stream& stream) {
  ASSIGN_OR_RETURN(auto& base_header, stream.peek<FrameBasicHeader>(), {});
  if (base_header.type() == FrameType::Base) {
    return pop_header_impl<FrameBasicHeader>(stream);
  }
  if (base_header.type() == FrameType::Extended16) {
    return pop_header_impl<FrameExtended16>(stream);
  }
  if (base_header.type() == FrameType::Extended64) {
    return pop_header_impl<FrameExtended64>(stream);
  }
  REQUIRE(false, "Unreachable");
}

template<typename HeaderType>
std::optional<FrameHeader>
peek_header_impl(base::Stream& stream) {
  ASSIGN_OR_RETURN(auto& header, stream.peek<HeaderType>(), {});
  return FrameHeader{
    .code = OpCode(header.code),
    .rsv3 = header.rsv3,
    .rsv2 = header.rsv2,
    .rsv1 = header.rsv1,
    .fin = header.fin,
    .mask = header.mask,
    .size = header.payload_size(),
    .header_size = sizeof(HeaderType)
  };
}

inline std::optional<FrameHeader>
peek_header(base::Stream& stream) {
  ASSIGN_OR_RETURN(auto& base_header, stream.peek<FrameBasicHeader>(), {});
  if (base_header.type() == FrameType::Base) {
    return peek_header_impl<FrameBasicHeader>(stream);
  }
  if (base_header.type() == FrameType::Extended16) {
    return peek_header_impl<FrameExtended16>(stream);
  }
  if (base_header.type() == FrameType::Extended64) {
    return peek_header_impl<FrameExtended64>(stream);
  }
  REQUIRE(false, "Unreachable");
}


} // namespace idk