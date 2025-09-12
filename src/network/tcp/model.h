#pragma once

#include "network/tcp/flags.h"
#include "network/type/big_endian.h"
#include "network/type/port.h"
#include "seq_number.h"


namespace idk::net::tcp {

#pragma pack(push, 1)

struct HeaderReflection {
  Port src_port;
  Port dst_port;
  SeqNumber seq;
  SeqNumber ack_seq;
  size_t data_offset;
  Flags flags;
  BE<uint16_t> window;
  BE<uint16_t> checksum;
  BE<uint16_t> urg_ptr;
};

struct Header {
  static constexpr bool kLoggable = true;

  using ReflectionType = HeaderReflection;

  [[nodiscard]] ReflectionType
  reflection() const;

  [[nodiscard]] size_t
  data_offset() const;

  Port src_port;
  Port dst_port;
  BE<SeqNumber> seq;
  BE<SeqNumber> ack;
  uint8_t reserved : 4, data_offset_scaled : 4;
  Flags flags;
  BE<uint16_t> window;
  BE<uint16_t> checksum;
  BE<uint16_t> urg_ptr;
};

static_assert(sizeof(Header) == 20);

struct OptionHeader {
  uint8_t kind;
  uint8_t length;
};

struct MssOption {
  OptionHeader header;
  BE<uint16_t> mss;
};

struct SackPermittedOption {
  OptionHeader header;
};

struct WindowScaleOption {
  OptionHeader header;
  uint8_t shift;
};

struct OptionsBlock {
  static constexpr uint8_t kNoOperationByte = 1;
  static constexpr uint8_t kTerminationByte = 0;
  MssOption mss;
  SackPermittedOption sack_permitted;
  WindowScaleOption window_scale;
  std::array<uint8_t, 3> term_and_padding_{kTerminationByte};
};

static_assert(sizeof(OptionsBlock) == 12);

#pragma pack(pop)

constexpr OptionHeader kMssOptionHeader = {.kind = 2, .length = 4};
constexpr OptionHeader kSackPermittedOptionHeader = {.kind = 4, .length = 2};
constexpr OptionHeader kWindowScaleOptionHeader = {.kind = 3, .length = 3};

constexpr MssOption kDefaultMssOption = {.header = kMssOptionHeader, .mss = 1460};
constexpr SackPermittedOption kSackPermittedOption = {.header = kSackPermittedOptionHeader};
constexpr WindowScaleOption kDefaultWindowScaleOption = {.header = kWindowScaleOptionHeader, .shift = 0};

constexpr OptionsBlock kDefaultOptionsBlock = {
  .mss = kDefaultMssOption,
  .sack_permitted = kSackPermittedOption,
  .window_scale = kDefaultWindowScaleOption
};

constexpr int kDefaultWindowSize = 65535;

} // namespace idk::net
