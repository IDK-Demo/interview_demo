#pragma once

#include "arp.h"
#include "base/logger/macros.h"
#include "base/type/span.h"
#include "network/dpdk/sender.h"
#include "network/type/endpoint.h"

namespace idk::net::arp {

class ArpHandler {
public:
  ArpHandler(Host local_host, dpdk::Sender sender);

  void
  handle_packet(base::MutableByteView raw_packet);

  void
  send_request(Ip target_ip);

private:
  Host local_host_;
  dpdk::Sender sender_;
};

} // namespace idk::net::dpdk