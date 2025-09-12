#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../base/stream/stream.h"
#include "flags.h"
#include "network/dpdk/sender.h"
#include "network/type/ip.h"
#include "network/type/mac.h"
#include "packet_view.h"
#include "seq_number.h"

namespace idk::net::tcp {

class Client {
public:
  static constexpr int kWindowSize = 65535;
  enum class State {
    Offline,
    Connecting,
    Connected,
  };

  Client(Connection connection, dpdk::Sender sender);

  Client(Client&&) = default;

  ~Client() {
    if (get_state() == State::Connected) {
      DEBUG("Sending RST packet");
      send_rst();
    }
  }

  struct SendBuffer {
    PacketView tcp;
    dpdk::TxPacket tx;
  };

  [[nodiscard]] SendBuffer
  get_send_buffer(Flags flags, bool include_options = false);

  void
  send(SendBuffer packet, size_t payload_len = 0);

  void
  process_packet(PacketView tcp_packet);

  void
  connect();

  void
  send_rst();

  State
  get_state() const {
    return state;
  }

  const Connection&
  get_connection() const {
    return connection;
  }
private:
  std::optional<dpdk::Sender> sender;

  uint8_t peer_window_scale;
  uint32_t unacknowledged_bytes;
  uint32_t send_wnd;
  SeqNumber last_ack_number;
  SeqNumber seq;
  SeqNumber ack;
  uint16_t ip_id;

  State state;
  Connection connection;
  uint16_t mss;
};


} // namespace idk
