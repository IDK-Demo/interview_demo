#pragma once

#include <optional>


#include "base/macros/require.h"
#include "base/type/default_constructor.h"
#include "base/type/span.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#include "rte_ether.h"
#include "rte_mbuf.h"
#pragma GCC diagnostic pop


namespace idk::net::dpdk {

class Device;

class RxPacket : base::NoCopy {
public:
  friend class Device;
  friend class std::optional<RxPacket>;

  RxPacket(RxPacket&& rhs) noexcept : rx_packet(rhs.rx_packet) { rhs.rx_packet = nullptr; }

  RxPacket&
  operator=(RxPacket&& rhs) noexcept {
    rx_packet = rhs.rx_packet;
    rhs.rx_packet = nullptr;
    return *this;
  }

  ~RxPacket() {
    if (rx_packet) {
      rte_pktmbuf_free(rx_packet);
    }
  }

  [[nodiscard]] base::MutableByteView
  bytes() const {
    auto* begin = rte_pktmbuf_mtod(rx_packet, uint8_t*);
    REQUIRE(begin, "Invalid state");
    return {rte_pktmbuf_mtod(rx_packet, uint8_t*), rx_packet->pkt_len};
  }

private:
  explicit RxPacket(rte_mbuf* rx_packet) : rx_packet(rx_packet) { REQUIRE(rx_packet, "Nullptr in rx_packet"); }

  rte_mbuf* rx_packet{nullptr};
};

static_assert(sizeof(RxPacket) == sizeof(rte_mbuf*));

class TxPacket : base::NoCopy {
public:
  friend class Device;
  friend class std::optional<TxPacket>;

  TxPacket(TxPacket&& rhs) noexcept : tx_packet(rhs.tx_packet) { rhs.tx_packet = nullptr; }

  TxPacket&
  operator=(TxPacket&& rhs) noexcept {
    tx_packet = rhs.tx_packet;
    rhs.tx_packet = nullptr;
    return *this;
  }

  ~TxPacket() {
    if (tx_packet) {
      rte_pktmbuf_free(tx_packet);
    }
  }

  [[nodiscard]] base::MutableByteView
  view() const {
    auto* begin = rte_pktmbuf_mtod(tx_packet, uint8_t*);
    REQUIRE(begin, "Invalid state");
    return {begin, rte_pktmbuf_data_room_size(tx_packet->pool)};
  }

private:
  explicit TxPacket(rte_mbuf* tx_packet) : tx_packet(tx_packet) {
    REQUIRE(tx_packet, "Nullptr in tx_packet");
    rte_pktmbuf_reset(tx_packet);
    tx_packet->l2_len = sizeof(rte_ether_hdr);
    tx_packet->l3_len = 20; // usually 20
    tx_packet->l4_len = 32;
    tx_packet->ol_flags |= RTE_MBUF_F_TX_IPV4 | RTE_MBUF_F_TX_IP_CKSUM | RTE_MBUF_F_TX_TCP_CKSUM;
  }

  rte_mbuf*
  release() {
    auto* ret = tx_packet;
    tx_packet = nullptr;
    return ret;
  }

  rte_mbuf* tx_packet{nullptr};
};

static_assert(sizeof(RxPacket) == sizeof(rte_mbuf*));
} // namespace idk::net::dpdk
