#pragma once
#include <string>
#include <vector>

#include "network/type/mac.h"
#include "network/type/ip.h"
#include "base/serialiser/auto/read_file.h"

#include "config.h"

namespace idk::net {

class InterfaceManager {
public:
  InterfaceManager(std::optional<std::string> config_path = {}) {
    if (config_path) {
      REQUIRE(!config_path->empty(), "path is empty");
      config = base::parse_file<InterfacesConfig>(config_path.value());
    } else {
      config = base::parse_file<InterfacesConfig>("/etc/idk/interfaces.yml");
    }
  }

  [[nodiscard]] const Interface& get_interface(const std::string& name) const {
    for (const auto& el : config.interfaces) {
      if (el.name == name) {
        return el;
      }
    }
    REQUIRE(false, "interface {} not found", name);
  }

private:
  InterfacesConfig config;
};

}  // namespace idk::base