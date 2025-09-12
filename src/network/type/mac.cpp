#include "mac.h"

#include <arpa/inet.h>

namespace idk::net {

Mac::Mac(const ReflectionType& str) {
  sscanf(str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac_bytes[0], &mac_bytes[1],
         &mac_bytes[2], &mac_bytes[3], &mac_bytes[4], &mac_bytes[5]);
}

Mac::ReflectionType
Mac::reflection() const {
  return as_string();
}

auto
Mac::data() const -> const uint8_t* {
  return mac_bytes;
}

auto
Mac::data() -> uint8_t* {
  return mac_bytes;
}

auto
Mac::as_string() const -> std::string {
  std::string mac_str;
  mac_str.resize(17); // 6*2 hex digits + 5 colons

  snprintf(mac_str.data(), mac_str.size() + 1, "%02X:%02X:%02X:%02X:%02X:%02X",
           mac_bytes[0], mac_bytes[1], mac_bytes[2],
           mac_bytes[3], mac_bytes[4], mac_bytes[5]);
  return mac_str;
}

bool
Mac::operator==(const Mac& other) const {
  return memcmp(mac_bytes, other.mac_bytes, kSize) == 0;
}

}
