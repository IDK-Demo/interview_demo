#pragma once
#include <string>
#include <vector>

#include "interface.h"

namespace idk::net {

struct InterfacesConfig {
  std::vector<Interface> interfaces;
};

}  // namespace idk::base