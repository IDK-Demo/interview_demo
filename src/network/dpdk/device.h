#pragma once
#include <string>


#include "base/clock/sync_rdtsc_clock.h"
#include "base/macros/require.h"
#include "network/type/mac.h"
#include "base/type/span.h"
#include "packet.h"
#include "sender.h"

namespace idk::base {
class Dpdk;
}

namespace idk::net::dpdk {

class Device : base::NoCopy {
public:
  Device(Device&&) = default;

  std::optional<RxPacket>
  receive();

  base::SyncRdtscClock::time_point get_last_receive_time_point() const;

  TxPacket
  get_send_buffer() const;

  void
  enqueue_for_send(TxPacket packet, size_t size);

  base::MutableByteView
  enqueue_for_send(size_t size);

  template<typename F>
  void
  enqueue_for_send(F&& f) {
    if (send_mbufs.size() == kSendBurstSize) {
      flush_send_queue();
    }
    rte_mbuf* mbuf = rte_pktmbuf_alloc(mbuf_pool);
    REQUIRE(mbuf, "Failed to allocate mbuf");
    rte_pktmbuf_reset(mbuf);
    mbuf->l2_len = sizeof(struct rte_ether_hdr);
    mbuf->l3_len = 20;
    mbuf->l4_len = 32;
    mbuf->ol_flags |= RTE_MBUF_F_TX_IPV4 | RTE_MBUF_F_TX_IP_CKSUM | RTE_MBUF_F_TX_TCP_CKSUM;
    send_mbufs.emplace_back(mbuf);
    auto size = f(rte_pktmbuf_mtod(mbuf, uint8_t*));
    mbuf->data_len = size;
    mbuf->pkt_len = size;
  }

  void flush_send_queue();

  Sender get_sender();

  [[nodiscard]] size_t
  clear_receive_queue();

  Mac mac() const;

  const std::string& pci_address() const;
  const std::string& interface_name() const;

  uint16_t port_id() const;

  struct Stats {
    static constexpr bool kLoggable = true;
    uint64_t ipackets;
    uint64_t opackets;
    uint64_t ibytes;
    uint64_t obytes;
    uint64_t imissed;
    uint64_t ierrors;
    uint64_t oerrors;
    uint64_t rx_nombuf;
    uint64_t q_ipackets;
    uint64_t q_opackets;
    uint64_t q_ibytes;
    uint64_t q_obytes;
    uint64_t q_errors;
  };

  [[nodiscard]] Stats
  stats() const;

  ~Device();

  Device(const std::string& pci_addr, uint16_t port_id, const std::string& interface_name);

private:
  friend Sender;
  friend class Dpdk;

  static constexpr uint16_t kReceiveBurstSize = 32;
  static constexpr uint16_t kSendBurstSize = 32;

  rte_mempool* mbuf_pool;
  std::string pci_addr;
  std::string interface_name_;
  uint16_t port_id_;
  uint16_t max_tx_packet_size{};

  // TODO StaticVector
  std::vector<rte_mbuf*> send_mbufs;
  std::vector<rte_mbuf*> receive_mbufs;
  size_t current_recieve_mbuf_idx{0};
  base::SyncRdtscClock::time_point last_receive_time_point{std::chrono::nanoseconds(0)};
};

} // namespace idk::net::dpdk
