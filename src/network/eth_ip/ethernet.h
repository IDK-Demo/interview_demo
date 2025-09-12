#pragma once
#include "network/type/mac.h"
#include "network/type/big_endian.h"
#include "base/type/span.h"
#include "base/type/start_lifetime_as.h"

namespace idk::net {

// The bytes are in reverse order.
// TODO make NetworkEnum class
enum class EthernetType : uint16_t { Ipv4 = 0x0800, Ipv6 = 0x86DD, Arp = 0x0806 };

#pragma pack(push, 1)

struct EthernetHeader {
  static constexpr bool kLoggable = true;
  Mac dst_mac;
  Mac src_mac;
  BE<EthernetType> type;
};

static_assert(sizeof(EthernetHeader) == 14);

#pragma pack(pop)

class EthernetPacketView {
public:
  explicit EthernetPacketView(base::MutableByteView bytes) : data(bytes) {}

  void
  init(Mac dst_mac, Mac src_mac, EthernetType type);

  static size_t
  predict_size(size_t payload_size);

  void
  resize_payload(size_t payload_size);

  [[nodiscard]] bool
  is_valid() const;

  [[nodiscard]] const EthernetHeader&
  header() const;

  EthernetHeader&
  header();

  [[nodiscard]] base::MutableByteView
  payload() const;

  [[nodiscard]] base::MutableByteView
  raw_bytes() const;

private:
  base::MutableByteView data;
};

} // namespace idk::net
