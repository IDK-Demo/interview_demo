#pragma once

#include <string>
#include <vector>
#include <cinttypes>

#include "network/wss/client.h"

namespace idk {

struct GatewayConfig {
  net::wss::Client::Config ws;

  size_t cpu_affinity;
  std::string interface;

  uint16_t src_port;
  uint16_t dst_port;
  net::Ip dst_ip;
};

}