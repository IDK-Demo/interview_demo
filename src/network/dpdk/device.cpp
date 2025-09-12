#include "device.h"
#include <rte_version.h>

#include "rte_eal.h"
#include "rte_ethdev.h"
#include "rte_ip.h"


namespace idk::net::dpdk {

Device::Device(const std::string& pci_addr, uint16_t port_id, const std::string& interface_name)
  : pci_addr(pci_addr), port_id_(port_id), interface_name_(interface_name) {
  auto name = fmt::format("pool_{}", port_id);
  mbuf_pool = rte_mempool_lookup(name.c_str());
  REQUIRE(mbuf_pool, "Failed to lookup mbuf_pool");
  REQUIRE(rte_eth_dev_get_mtu(port_id, &max_tx_packet_size) == 0, "Failed to get max_tx_packet_size");

  send_mbufs.reserve(kSendBurstSize);
  receive_mbufs.reserve(kReceiveBurstSize);
  INFO("Device {} on port {} initialized", pci_addr, port_id);
}

std::optional<RxPacket>
Device::receive() {
  std::optional<RxPacket> ret;
  if (current_recieve_mbuf_idx == receive_mbufs.size()) {
    receive_mbufs.resize(kReceiveBurstSize);
    auto cnt = rte_eth_rx_burst(port_id_, 0, receive_mbufs.data(), receive_mbufs.size());
    receive_mbufs.resize(cnt);
    current_recieve_mbuf_idx = 0;
    last_receive_time_point = base::SyncRdtscClock::now();
  }
  if (current_recieve_mbuf_idx < receive_mbufs.size()) {
    return RxPacket(receive_mbufs[current_recieve_mbuf_idx++]);
  }
  return std::nullopt;
}

TxPacket
Device::get_send_buffer() const {
  TxPacket packet(rte_pktmbuf_alloc(mbuf_pool));
  return packet;
}

void
Device::enqueue_for_send(TxPacket packet, size_t size) {
  auto* tx_packet = packet.release();
  tx_packet->data_len = size;
  tx_packet->pkt_len = size;
  REQUIRE(rte_eth_tx_burst(port_id_, 0, &tx_packet, 1) == 1, "failed to send");
}

base::SyncRdtscClock::time_point
Device::get_last_receive_time_point() const {
  REQUIRE(last_receive_time_point.time_since_epoch().count() != 0,
          "get_last_receive_time_point can be only called after a received");
  return last_receive_time_point;
}

Sender
Device::get_sender() {
  return Sender(this);
}

base::MutableByteView
Device::enqueue_for_send(size_t size) {
  if (send_mbufs.size() == kSendBurstSize) {
    flush_send_queue();
  }
  rte_mbuf* mbuf = rte_pktmbuf_alloc(mbuf_pool);
  REQUIRE(mbuf, "Failed to allocate mbuf");
  send_mbufs.emplace_back(mbuf);
  char* pkt_data = rte_pktmbuf_append(mbuf, size);
  return {reinterpret_cast<uint8_t*>(pkt_data), size};
}

void
Device::flush_send_queue() {
  if (send_mbufs.empty()) {
    return;
  }
  uint16_t sent = rte_eth_tx_burst(port_id_, 0, send_mbufs.data(), send_mbufs.size());

  if (sent < send_mbufs.size()) {
    DEBUG("sent: {} of {}", sent, send_mbufs.size());
    std::copy(send_mbufs.begin() + sent, send_mbufs.end(), send_mbufs.begin());
  }
  send_mbufs.resize(send_mbufs.size() - sent);
}

size_t
Device::clear_receive_queue() {
  size_t counter = 0;
  while (receive()) {
    ++counter;
  }
  return counter;
}

Mac
Device::mac() const {
  Mac mac{};
  rte_eth_macaddr_get(port_id_, reinterpret_cast<rte_ether_addr*>(&mac));
  return mac;
}

const std::string&
Device::pci_address() const {
  return pci_addr;
}

const std::string&
Device::interface_name() const {
  return interface_name_;
}


uint16_t
Device::port_id() const {
  return port_id_;
}

Device::Stats
Device::stats() const {
  rte_eth_stats rte_stats{};
  REQUIRE(rte_eth_stats_get(port_id_, &rte_stats) == 0, "failed to call rte_eth_stats_get() on port {}", port_id_);

  return {
      .ipackets = rte_stats.ipackets,
      .opackets = rte_stats.opackets,
      .ibytes = rte_stats.ibytes,
      .obytes = rte_stats.obytes,
      .imissed = rte_stats.imissed,
      .ierrors = rte_stats.ierrors,
      .oerrors = rte_stats.oerrors,
      .rx_nombuf = rte_stats.rx_nombuf,
      .q_ipackets = rte_stats.q_ipackets[0],
      .q_opackets = rte_stats.q_opackets[0],
      .q_ibytes = rte_stats.q_ibytes[0],
      .q_obytes = rte_stats.q_obytes[0],
      .q_errors = rte_stats.q_errors[0],
  };
}

Device::~Device() {
  for (auto& m: send_mbufs) {
    rte_pktmbuf_free(m);
  }
  // FIXME segfaults
  // for (auto& m: receive_mbufs) {
  //   rte_pktmbuf_free(m);
  // }
}
} // namespace idk::net::dpdk
