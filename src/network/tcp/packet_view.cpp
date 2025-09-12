#include "base/type/align.h"
#include "packet_view.h"
#include <rte_ip.h>

namespace idk::net::tcp {

HeaderReflection
Header::reflection() const {
  return ReflectionType{src_port, dst_port, seq.value(), ack.value(), data_offset(), flags, window, checksum, urg_ptr};
}

size_t
Header::data_offset() const {
  return data_offset_scaled * 4;
}

size_t PacketView::predict_size(size_t payload_size, bool options) {
  return IPPacketView::predict_size(sizeof(Header) + (options ? 12 : 0) + payload_size);
}

void
PacketView::resize_payload(size_t payload_size) {
  ip_packet_view.resize_payload(header().data_offset() + payload_size);
}

void
PacketView::init(Connection connection, Flags flags, SeqNumber seq, SeqNumber ack_seq, bool options) {
  ip_packet_view.init(connection.session, IpProtocol::Tcp);

  Header& hdr = header();
  hdr.src_port = connection.src_port;
  hdr.dst_port = connection.dst_port;
  hdr.data_offset_scaled = sizeof(Header) / 4;
  hdr.flags = flags;
  hdr.reserved = 0;
  hdr.window = kDefaultWindowSize;
  hdr.checksum = 0; // To be calculated later
  hdr.urg_ptr = 0;
  hdr.seq = seq;
  hdr.ack = ack_seq;

  if (options) {
    hdr.data_offset_scaled += sizeof(OptionsBlock) / 4;
    *base::start_lifetime_as<OptionsBlock>((uint8_t*)(&hdr + 1)) = kDefaultOptionsBlock;
  }
}

BE<uint16_t> PacketView::calc_checksum() const {
  auto& tcp_hdr = const_cast<Header&>(header());
  uint16_t cksum = rte_ipv4_udptcp_cksum(
    reinterpret_cast<const rte_ipv4_hdr*>(&ip().header()),
    &tcp_hdr
  );
  return BE<uint16_t>::from_big_endian(cksum);
}

void PacketView::update_checksum() {
  header().checksum = calc_checksum();
}

base::MutableByteView
PacketView::payload() {
  const auto ip_payload = ip_packet_view.payload();
  const auto tcp_header = header();
  return {ip_payload.data() + tcp_header.data_offset(), ip_payload.size() - tcp_header.data_offset()};
}

base::ByteView
PacketView::payload() const {
  const auto ip_payload = ip_packet_view.payload();
  const auto tcp_header = header();
  return {ip_payload.data() + tcp_header.data_offset(), ip_payload.size() - tcp_header.data_offset()};
}

bool
PacketView::is_valid() const {
  return ip_packet_view.is_valid();
}

EthernetPacketView
PacketView::eth() {
  return ip_packet_view.eth();
}

IPPacketView
PacketView::ip() const {
  return ip_packet_view;
}

Header&
PacketView::header() {
  return *base::start_lifetime_as<Header>(ip_packet_view.payload().data());
}

const Header&
PacketView::header() const {
  return *base::start_lifetime_as<Header>(ip_packet_view.payload().data());
}

PacketView::Options
PacketView::parse_options() const {
  PacketView::Options options{};
  const Header& tcp_header = header();

  const uint8_t* options_data = reinterpret_cast<const uint8_t*>(&tcp_header + 1);
  const size_t options_length = tcp_header.data_offset() - sizeof(Header);
  size_t offset = 0;

  while (offset < options_length) {
    const uint8_t option_kind = options_data[offset];

    if (option_kind == OptionsBlock::kTerminationByte) {
      break;
    }
    if (option_kind == OptionsBlock::kNoOperationByte) {
      offset += 1;
      continue;
    }
    REQUIRE(offset + sizeof(OptionHeader) <= options_length, "Not enough space for option header");
    const auto* option_hdr = reinterpret_cast<const OptionHeader*>(&options_data[offset]);
    REQUIRE(option_hdr->length >= sizeof(OptionHeader) && offset + option_hdr->length <= options_length, "Invalid length field");

    switch (option_kind) {
      case kWindowScaleOptionHeader.kind:
        if (option_hdr->length == sizeof(WindowScaleOption)) {
          const auto* ws_option = reinterpret_cast<const WindowScaleOption*>(option_hdr);
          options.window_scale = ws_option->shift;
        }
        break;

      default:
        break;
    }
    offset += option_hdr->length;
  }

  return options;
}

} // namespace idk::net
