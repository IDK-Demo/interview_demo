#pragma once

#include "config.h"

#include "../../network/arp/arp_handler.h"
#include "base/launcher/tool_launcher.h"
#include "network/dpdk/dpdk.h"
#include "network/interface/interface.h"
#include "network/wss/client.h"

namespace idk {

class WsDemoServiceImpl {
public:
  struct Args {
    static constexpr std::string_view kBinaryName = "gateway";
    static constexpr std::string_view kHelp = "Dpdk websocket demo";
    static constexpr std::string_view kVersion = "0.1";
    base::arg::Argument<std::string, "path to config"> config;
    base::arg::Argument<std::optional<std::string>, "path to interfaces config"> interface_config;
  };

  WsDemoServiceImpl(Args args, base::ToolLauncherContext* ctx);

  using Config = GatewayConfig;

  int
  run();

private:
  void process_payload(base::ByteView payload);

private:
  Args args;
  base::ToolLauncherContext* ctx;

  std::optional<net::Dpdk> dpdk;
  std::optional<net::arp::ArpHandler> arp_handler;

  std::optional<net::wss::Client> connection;
  std::vector<uint64_t> per_instrument_last_json_id;

  net::Interface interface;
  net::InterfaceManager interface_manager;
};

using WsDemoService = base::ToolLauncher<WsDemoServiceImpl>;

} // namespace idk
