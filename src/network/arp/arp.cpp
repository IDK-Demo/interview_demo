#include "arp.h"

#include "base/macros/require.h"

namespace idk::net::arp {

HeaderReflection
Header::reflection() const {
  return ReflectionType{hardware_type.value(),
                        protocol_type.value(),
                        hardware_len,
                        protocol_len,
                        operation.value(),
                        sender_mac,
                        sender_ip,
                        target_mac,
                        target_ip};
}

void
PacketView::init_request(Mac sender_mac, Ip sender_ip, Mac target_mac, Ip target_ip) {
  eth().init(target_mac, sender_mac, EthernetType::Arp);

  Header& hdr = *base::start_lifetime_as<Header>(eth_view.payload().data());
  hdr.hardware_type = HardwareType::Ethernet;
  hdr.protocol_type = ProtocolType::Ipv4;
  hdr.hardware_len = 6; // MAC address length
  hdr.protocol_len = 4; // IPv4 address length
  hdr.operation = Operation::Request;
  hdr.sender_mac = sender_mac;
  hdr.sender_ip = sender_ip;
  hdr.target_mac = target_mac;
  hdr.target_ip = target_ip;
}

void
PacketView::init_reply(Mac sender_mac, Ip sender_ip, Mac target_mac, Ip target_ip) {
  eth().init(target_mac, sender_mac, EthernetType::Arp);

  Header& hdr = *base::start_lifetime_as<Header>(eth_view.payload().data());
  hdr.hardware_type = HardwareType::Ethernet;
  hdr.protocol_type = ProtocolType::Ipv4;
  hdr.hardware_len = 6; // MAC address length
  hdr.protocol_len = 4; // IPv4 address length
  hdr.operation = Operation::Reply;
  hdr.sender_mac = sender_mac;
  hdr.sender_ip = sender_ip;
  hdr.target_mac = target_mac;
  hdr.target_ip = target_ip;
}

size_t
PacketView::predict_size() {
  return EthernetPacketView::predict_size(sizeof(Header));
}

bool
PacketView::is_valid() const {
  const bool is_valid =
      eth_view.payload().size() >= sizeof(Header) && header().hardware_type == HardwareType::Ethernet &&
      header().protocol_type == ProtocolType::Ipv4 && header().hardware_len == 6 && header().protocol_len == 4 &&
      (header().operation == Operation::Request || header().operation == Operation::Reply);
  return is_valid;
}

const Header&
PacketView::header() const {
  return *base::start_lifetime_as<Header>(eth_view.payload().data());
}

Header&
PacketView::header() {
  return *base::start_lifetime_as<Header>(eth_view.payload().data());
}

EthernetPacketView
PacketView::eth() {
  return eth_view;
}

} // namespace idk::net::arp
