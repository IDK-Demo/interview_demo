#pragma once
#include <optional>
#include "base/stream/mutable_byte_view_stream.h"
#include "network/tcp/flags.h"
#include "network/type/big_endian.h"
#include "network/type/mac.h"
#include "network/type/port.h"
#include "seq_number.h"

#include "../eth_ip/ip.h"
#include "model.h"

namespace idk::net::tcp {

class PacketView {
public:
  explicit PacketView(base::MutableByteView bytes) : ip_packet_view(bytes) {}

  explicit PacketView(IPPacketView bytes) : ip_packet_view(bytes) {}

  void
  init(Connection connection, Flags flags, SeqNumber seq, SeqNumber ack_seq, bool options = false);

  void
  update_checksum();

  BE<uint16_t>
  calc_checksum() const;

  static size_t
  predict_size(size_t payload_size, bool options);

  void
  resize_payload(size_t payload_size);

  [[nodiscard]] base::MutableByteView
  payload();

  [[nodiscard]] base::ByteView
  payload() const;

  [[nodiscard]] base::MutableByteViewWriteStream
  payload_write_stream() {
    return base::MutableByteViewWriteStream{payload()};
  }

  [[nodiscard]] bool
  is_valid() const;

  EthernetPacketView
  eth();

  [[nodiscard]] IPPacketView
  ip() const;

  Header&
  header();

  [[nodiscard]] const Header&
  header() const;

  struct Options {
    std::optional<uint8_t> window_scale;
  };
  [[nodiscard]] Options
  parse_options() const;

private:
  IPPacketView ip_packet_view;
};

} // namespace idk::base
