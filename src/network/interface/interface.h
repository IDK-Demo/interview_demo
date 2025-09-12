#pragma once
#include <string>
#include <vector>

#include "network/type/mac.h"
#include "network/type/ip.h"

namespace idk::net {

struct Interface {
  std::string name;
  std::string pci;
  std::string driver;
  Mac mac;
  Ip ip;
  Mac gateway_mac;

  std::string gateway_name = "default";
};

}  // namespace idk::base