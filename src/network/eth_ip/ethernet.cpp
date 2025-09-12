#include "ethernet.h"
#include "base/macros/require.h"

namespace idk::net {

void
EthernetPacketView::init(Mac dst_mac, Mac src_mac, EthernetType type) {
  header().dst_mac = dst_mac;
  header().src_mac = src_mac;
  header().type = type;
  REQUIRE(is_valid(), "ethernet packet is not valid");
}

size_t
EthernetPacketView::predict_size(size_t payload_size) {
  return sizeof(EthernetHeader) + payload_size;
}

void
EthernetPacketView::resize_payload(size_t payload_size) {
  data = base::MutableByteView{data.data(), sizeof(EthernetHeader) + payload_size};
}

bool
EthernetPacketView::is_valid() const {
  return data.size() >= sizeof(EthernetHeader);
}

const EthernetHeader&
EthernetPacketView::header() const {
  return *base::start_lifetime_as<EthernetHeader>(data.data());
}

EthernetHeader&
EthernetPacketView::header() {
  return *base::start_lifetime_as<EthernetHeader>(data.data());
}

base::MutableByteView
EthernetPacketView::payload() const {
  return {data.data() + sizeof(EthernetHeader), data.size() - sizeof(EthernetHeader)};
}

base::MutableByteView
EthernetPacketView::raw_bytes() const {
  return data;
}

} // namespace idk::net
