#pragma once
#include "../eth_ip/ethernet.h"
#include "base/type/span.h"
#include "base/type/start_lifetime_as.h"
#include "network/type/big_endian.h"
#include "network/type/ip.h"
#include "network/type/mac.h"

namespace idk::net::arp {

#pragma pack(push, 1)

enum class HardwareType : uint16_t {
  Ethernet = 1
};

enum class ProtocolType : uint16_t {
  Ipv4 = 0x0800
};

enum class Operation : uint16_t {
  Request = 1,
  Reply = 2
};

struct HeaderReflection {
  HardwareType hardware_type;
  ProtocolType protocol_type;
  uint8_t hardware_len;
  uint8_t protocol_len;
  Operation operation;
  Mac sender_mac;
  Ip sender_ip;
  Mac target_mac;
  Ip target_ip;
};

struct Header {
  static constexpr bool kLoggable = true;

  using ReflectionType = HeaderReflection;

  [[nodiscard]] ReflectionType
  reflection() const;

  BE<HardwareType> hardware_type;
  BE<ProtocolType> protocol_type;
  uint8_t hardware_len;
  uint8_t protocol_len;
  BE<Operation> operation;
  Mac sender_mac;
  Ip sender_ip;
  Mac target_mac;
  Ip target_ip;
};

static_assert(sizeof(Header) == 28);

#pragma pack(pop)

class PacketView {
public:
  explicit PacketView(base::MutableByteView data) : eth_view(data) {}

  explicit PacketView(EthernetPacketView data) : eth_view(data) {}

  void
  init_request(Mac sender_mac, Ip sender_ip, Mac target_mac, Ip target_ip);

  void
  init_reply(Mac sender_mac, Ip sender_ip, Mac target_mac, Ip target_ip);

  static size_t
  predict_size();

  [[nodiscard]] bool
  is_valid() const;

  [[nodiscard]] const Header&
  header() const;

  Header&
  header();

  EthernetPacketView
  eth();

private:
  EthernetPacketView eth_view;
};

} // namespace idk::base