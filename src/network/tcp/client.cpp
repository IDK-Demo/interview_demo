#include "client.h"

#include <arpa/inet.h>
#include <random>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "packet_view.h"

namespace idk::net::tcp {

Client::Client(Connection connection,  dpdk::Sender sender) :
    connection(connection), sender(sender), send_wnd(kWindowSize) {
  state = State::Offline;
  unacknowledged_bytes = 0;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
  std::uniform_int_distribution<uint16_t> dist16(0, UINT16_MAX);

  seq = dist(gen);
  ack = 0;
  ip_id = dist16(gen);
  last_ack_number = seq;
  peer_window_scale = 0;
}

Client::SendBuffer
Client::get_send_buffer(Flags flags, bool include_options) {
  auto tx = sender->get_send_buffer();
  auto tcp = PacketView{tx.view()};
  tcp.init(connection, flags, seq, ack, include_options);
  tcp.ip().header().id = ip_id;
  return {.tcp = tcp, .tx = std::move(tx)};
}

void
Client::send(SendBuffer packet, size_t payload_len) {
  REQUIRE(unacknowledged_bytes + payload_len <= send_wnd, "window is full");

  auto tcp = packet.tcp;
  tcp.resize_payload(payload_len);
  tcp.update_checksum();
  tcp.ip().update_checksum();

  ip_id++;
  seq += payload_len;
  unacknowledged_bytes += payload_len;

  sender->send_raw(std::move(packet.tx), tcp.eth().raw_bytes().size());
}

void
Client::process_packet(PacketView tcp_packet) {
  REQUIRE(tcp_packet.is_valid(), "Tcp packet is not valid");
  auto& hdr = tcp_packet.header();
  REQUIRE(!has_flag(hdr.flags, Flags::RST), "rst received");

  auto received_seq = hdr.seq.value();
  auto received_ack = hdr.ack.value();
  if (received_seq < ack && ack != 0) [[unlikely]] {
    return;
  }
  REQUIRE(received_seq == ack || ack == 0, "Misses are not supported");

  uint32_t peer_rwnd = hdr.window.value() << peer_window_scale;
  if (peer_rwnd > 0) {
    send_wnd = peer_rwnd;
  }

  REQUIRE_LE(last_ack_number, received_ack, "ack is not expected to decrease");
  uint32_t bytes_acked = received_ack - last_ack_number;
  REQUIRE_LE(bytes_acked, unacknowledged_bytes, "Unexpected bytes got acknowledged");
  unacknowledged_bytes -= bytes_acked;
  last_ack_number = received_ack;

  if (state == State::Connecting && hdr.flags == Flags::SYN_ACK) [[unlikely]] {
    DEBUG("State::CONNECTED");
    state = State::Connected;
    ack = received_seq + 1;
    seq += 1;
    last_ack_number = received_ack;

    auto options = tcp_packet.parse_options();
    if (options.window_scale.has_value()) {
      peer_window_scale = options.window_scale.value();
      DEBUG("Window scale received from server: {}", peer_window_scale);
    }
    send(get_send_buffer(Flags::ACK));
    return;
  }
  if (state == State::Connected && !tcp_packet.payload().empty()) {
    ack = received_seq + tcp_packet.payload().size();
    send(get_send_buffer(Flags::ACK));
  }
}

void
Client::connect() {
  auto packet = sender->get_send_buffer();
  state = State::Connecting;
  send(get_send_buffer(Flags::SYN, true), 0);
  unacknowledged_bytes = 1;
  DEBUG("syn sent");
}

void
Client::send_rst() {
  auto packet = sender->get_send_buffer();
  DEBUG("Sending RST");
  send(get_send_buffer(Flags::RST));
  state = State::Offline;
}

} // namespace idk::base
