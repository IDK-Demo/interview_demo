#pragma once

#include "ip.h"
#include "mac.h"
#include "port.h"

namespace idk::net {

struct Host {
  static constexpr bool kLoggable = true;
  Mac mac;
  Ip ip;

  bool operator==(const Host& other) const {
    return mac == other.mac && ip == other.ip;
  }
};

struct Endpoint {
  static constexpr bool kLoggable = true;

  Ip ip;
  Port port;

  bool operator==(const Endpoint& other) const {
    return ip == other.ip && port == other.port;
  }
};

struct Session {
  static constexpr bool kLoggable = true;

  Host src;
  Host dst;

  bool operator==(const Session& other) const {
    return src == other.src && dst == other.dst;
  }
};

struct Connection {
  static constexpr bool kLoggable = true;

  Session session;
  Port src_port;
  Port dst_port;

  bool operator==(const Connection& other) const {
    return session == other.session && src_port == other.src_port && dst_port == other.dst_port;
  }
};


}