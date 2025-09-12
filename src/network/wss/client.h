#pragma once

#include <utility>

#include "base/logger/logger.h"
#include "base/type/span.h"
#include "network/dpdk/sender.h"
#include "network/tcp/client.h"
#include "network/tls12/client.h"

#include "model.h"

namespace idk::net::wss {

class Client : public base::LoggableComponent, public base::NoCopy {
public:
  struct Config {
    std::string host;
    std::string path;
  };
  enum class State : uint8_t { Init = 0, HttpHandshake, Connected };

  explicit Client(Config config_ws, tcp::Client tcp_client) : LoggableComponent("ws"), config(std::move(config_ws)) {
    tls.emplace(config.host, std::move(tcp_client));
  }

  Client(Client&&) = default;

  struct Result {
    OpCode code;
    base::MutableByteView bytes;
  };
  std::optional<Result>
  extract_payload();

  void
  generate_upgrade_request();

  bool
  accept_handshake();

  void
  process_packet(tcp::PacketView tcp_packet) {
    tls->process_packet(tcp_packet);
  }

  std::optional<base::ByteView>
  next_message();


private:
  std::optional<tls12::Client> tls;
  base::Stream stream{200000};

  State state{State::Init};

  std::optional<dpdk::Sender> sender;
  Config config;
};


} // namespace idk::net
