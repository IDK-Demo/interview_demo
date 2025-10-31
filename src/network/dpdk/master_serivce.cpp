#include "master_service.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

namespace idk::base {

void
DpdkMasterServiceImpl::init() {
  char* argv[] = {"dpdk_primary -l 0 --proc-type=primary"};
  REQUIRE_EQ(rte_eal_init(1, argv), 0, "");

  uint16_t mbuf_size = RTE_MBUF_DEFAULT_BUF_SIZE;

  auto ports = rte_eth_dev_count_avail();
  DEBUG("Available ports: {}", ports);

  int i = 0;
  uint16_t port_id;
  RTE_ETH_FOREACH_DEV(port_id) {
    rte_eth_dev_info dev_info;
    REQUIRE_EQ(rte_eth_dev_info_get(port_id, &dev_info), 0, "");
    DEBUG("Initializing port {}:", port_id);
    DEBUG("  driver_name: {}", dev_info.driver_name);
    DEBUG("  min_mtu: {}", dev_info.min_mtu);
    DEBUG("  max_mtu: {}", dev_info.max_mtu);
    DEBUG("  min_rx_bufsize: {}", dev_info.min_rx_bufsize);
    DEBUG("  max_rx_bufsize: {}", dev_info.max_rx_bufsize);
    DEBUG("  max_rx_queues: {}", dev_info.max_rx_queues);
    DEBUG("  max_tx_queues: {}", dev_info.max_tx_queues);

    // Use minimum of requested (2) and supported queues
    uint16_t nb_rx_queues = std::min<uint16_t>(2, dev_info.max_rx_queues);
    uint16_t nb_tx_queues = std::min<uint16_t>(2, dev_info.max_tx_queues);
    INFO("Configuring port {} with {} RX queues and {} TX queues", port_id, nb_rx_queues, nb_tx_queues);

    rte_eth_conf port_conf{};
    // Enable RSS to distribute packets across queues based on 5-tuple hash
    if (nb_rx_queues > 1) {
      port_conf.rxmode.mq_mode = RTE_ETH_MQ_RX_RSS;
      port_conf.rx_adv_conf.rss_conf.rss_key = NULL;  // Use default RSS key
      // Use RSS hash flags supported by the device
      uint64_t rss_hf = RTE_ETH_RSS_IP | RTE_ETH_RSS_TCP | RTE_ETH_RSS_UDP;
      port_conf.rx_adv_conf.rss_conf.rss_hf = rss_hf & dev_info.flow_type_rss_offloads;
      INFO("  Enabling RSS for packet distribution across {} queues (rss_hf: 0x{:x})",
           nb_rx_queues, port_conf.rx_adv_conf.rss_conf.rss_hf);
    }
    REQUIRE_EQ(rte_eth_dev_configure(port_id, nb_rx_queues, nb_tx_queues, &port_conf), 0, "");

    uint16_t nb_rxd = 512;
    uint16_t nb_txd = 512;
    REQUIRE_EQ(rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &nb_rxd, &nb_txd), 0, "");

    auto name = fmt::format("pool_{}", port_id);
    ctx.mbuf_pools[i] =
        rte_pktmbuf_pool_create(name.c_str(), 16384, 256, 0, mbuf_size, rte_socket_id());
    REQUIRE(ctx.mbuf_pools[i], "");

    // Setup RX queues
    for (uint16_t queue_id = 0; queue_id < nb_rx_queues; queue_id++) {
      REQUIRE_EQ(rte_eth_rx_queue_setup(port_id, queue_id, nb_rxd, rte_eth_dev_socket_id(port_id),
                                        &dev_info.default_rxconf, ctx.mbuf_pools[i]),
                 0, "Failed to setup RX queue {} on port {}", queue_id, port_id);
      DEBUG("  Setup RX queue {}", queue_id);
    }

    // Setup TX queues
    for (uint16_t queue_id = 0; queue_id < nb_tx_queues; queue_id++) {
      REQUIRE_EQ(rte_eth_tx_queue_setup(port_id, queue_id, nb_txd, rte_eth_dev_socket_id(port_id),
                                        &dev_info.default_txconf),
                 0, "Failed to setup TX queue {} on port {}", queue_id, port_id);
      DEBUG("  Setup TX queue {}", queue_id);
    }

    REQUIRE_EQ(rte_eth_dev_start(port_id), 0, "");
    port_ids.push_back(port_id);

    INFO("Initialized port {}", port_id);
    i++;
  }
}

DpdkMasterServiceImpl::~DpdkMasterServiceImpl() {
  for (int i = 0; i < port_ids.size(); i++) {
    rte_eth_dev_stop(port_ids[i]);
    rte_eth_dev_close(port_ids[i]);
    if (ctx.mbuf_pools[i]) {
      rte_mempool_free(ctx.mbuf_pools[i]);
    }
  }
  rte_eal_cleanup();
}


int
DpdkMasterServiceImpl::run() {
  init();
  while (!tool_ctx->is_stopped()) {
    tool_ctx->wait_for_signal();
  }
  return 0;
}
} // namespace idk::base
