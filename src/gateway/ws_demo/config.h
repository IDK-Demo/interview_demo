#pragma once

#include <string>
#include <vector>
#include <cinttypes>

#include "network/wss/client.h"

namespace idk {

struct InstanceConfig {
  static constexpr bool kLoggable = true;
  net::wss::Client::Config ws;

  size_t cpu_affinity;
  uint16_t queue_id;
  std::string interface;

  uint16_t src_port;
  uint16_t dst_port;
  net::Ip dst_ip;
};

struct GatewayConfig {
  static constexpr bool kLoggable = true;
  std::vector<InstanceConfig> instances;
};

}