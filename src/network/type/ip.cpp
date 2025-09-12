#include "ip.h"
#include <arpa/inet.h>

namespace idk::net {

Ip::Ip(const std::string& str) {
  inet_pton(AF_INET, str.c_str(), &addr);
}

Ip::Ip(uint32_t data_) {
  data() = data_;
}

Ip::ReflectionType
Ip::reflection() const {
  return as_string();
}

const uint32_t&
Ip::data() const {
  return reinterpret_cast<const uint32_t&>(addr);
}

uint32_t&
Ip::data() {
  return reinterpret_cast<uint32_t&>(addr);
}

auto
Ip::as_string() const -> std::string {
  std::string ip;
  ip.resize(INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &addr, ip.data(), INET_ADDRSTRLEN);
  ip.resize(std::strlen(ip.data()));
  return ip;
}

bool
Ip::operator<(const Ip& rhs) const {
  return reinterpret_cast<const uint32_t&>(addr) < reinterpret_cast<const uint32_t&>(rhs.addr);
}

bool
Ip::operator==(const Ip& rhs) const {
  return reinterpret_cast<const uint32_t&>(addr) == reinterpret_cast<const uint32_t&>(rhs.addr);
}

}
