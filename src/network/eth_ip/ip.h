#pragma once
#include "network/type/big_endian.h"
#include "network/type/endpoint.h"
#include "network/type/ip.h"
#include "network/type/mac.h"
#include "base/type/span.h"
#include "base/type/start_lifetime_as.h"
#include "ethernet.h"

namespace idk::net {


#pragma pack(push, 1)

enum class IpVersion : uint8_t { IPv4 = 4, IPv6 = 6 };

enum class IpProtocol : uint8_t {
  Tcp = 6,
  Udp = 17
};

struct IpHeaderReflection {
  IpVersion version;
  size_t header_size;
  uint8_t tos;
  BE<uint16_t> total_length;
  BE<uint16_t> id;
  BE<uint16_t> frag_off;
  uint8_t ttl;
  IpProtocol protocol;
  BE<uint16_t> check;
  Ip src_addr;
  Ip dst_addr;
};

struct IpHeader {
  static constexpr bool kLoggable = true;

  using ReflectionType = IpHeaderReflection;

  [[nodiscard]] ReflectionType
  reflection() const;

  size_t
  size() const;

  uint8_t ihl : 4, version : 4;
  uint8_t tos;
  BE<uint16_t> total_length;
  BE<uint16_t> id;
  BE<uint16_t> frag_off;
  uint8_t ttl;
  IpProtocol protocol;
  BE<uint16_t> check;
  Ip src_addr;
  Ip dst_addr;
};

static_assert(sizeof(IpHeader) == 20);

#pragma pack(pop)

class IPPacketView {
public:
  explicit IPPacketView(base::MutableByteView data) : eth_view(data) {}

  explicit IPPacketView(EthernetPacketView data) : eth_view(data) {}

  void
  init(Session session, IpProtocol protocol);

  void update_checksum();

  static size_t
  predict_size(size_t payload_size);

  void
  resize_payload(size_t payload_size);

  [[nodiscard]] bool
  is_valid() const;

  [[nodiscard]] const IpHeader&
  header() const;

  IpHeader&
  header();

  EthernetPacketView
  eth();

  [[nodiscard]] base::MutableByteView
  payload() const;

private:
  EthernetPacketView eth_view;
};

} // namespace idk::base
