#include "ip.h"

#include "base/macros/require.h"
#include "rte_ip4.h"

namespace idk::net {

IpHeaderReflection
IpHeader::reflection() const {
  return ReflectionType{IpVersion{version}, size(), tos,      total_length, id, frag_off, ttl,
                        protocol,           check,  src_addr, dst_addr};
}
size_t
IpHeader::size() const {
  return ihl * 4;
}

void
IPPacketView::init(Session session, IpProtocol protocol) {
  eth().init(session.dst.mac, session.src.mac, EthernetType::Ipv4);

  IpHeader& hdr = *base::start_lifetime_as<IpHeader>(eth_view.payload().data());
  hdr.version = 4;
  hdr.ihl = 5;
  hdr.tos = 0;
  hdr.total_length = eth_view.payload().size();
  hdr.id = 0;
  hdr.frag_off = 0;
  hdr.ttl = 64;
  hdr.protocol = protocol;
  hdr.src_addr = session.src.ip;
  hdr.dst_addr = session.dst.ip;
  hdr.check = 0;
  REQUIRE(is_valid(), "ip packet is not valid");
}

void
IPPacketView::update_checksum() {
  header().check = BE<uint16_t>::from_big_endian(rte_ipv4_cksum(reinterpret_cast<rte_ipv4_hdr*>(&header())));
}

size_t
IPPacketView::predict_size(size_t payload_size) {
  return EthernetPacketView::predict_size(20 + payload_size);
}

void
IPPacketView::resize_payload(size_t payload_size) {
  eth_view.resize_payload(header().size() + payload_size);
  header().total_length = eth_view.payload().size();
}

bool
IPPacketView::is_valid() const {
  const auto ip_size = header().total_length.value();
  const auto eth_payload_size = eth_view.payload().size();

  const bool is_valid = (ip_size < 60 || ip_size == eth_payload_size) && header().size() <= eth_view.payload().size();

  if (!is_valid) [[unlikely]] {
    TRACE_L2("eth_view.payload().size(): {}", eth_view.payload().size());
    TRACE_L2("header().size(): {}", header().size());
    TRACE_L2("header().total_length.as_little_endian(): {}", header().total_length.value());
  }
  return is_valid;
}

const IpHeader&
IPPacketView::header() const {
  return *base::start_lifetime_as<IpHeader>(eth_view.payload().data());
}

IpHeader&
IPPacketView::header() {
  return *base::start_lifetime_as<IpHeader>(eth_view.payload().data());
}

EthernetPacketView
IPPacketView::eth() {
  return eth_view;
}

base::MutableByteView
IPPacketView::payload() const {
  return {eth_view.payload().data() + header().size(), header().total_length.value() - header().size()};
}

} // namespace idk::net
