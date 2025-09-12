#pragma once

#include "base/launcher/tool_launcher.h"

#include <rte_ethdev.h>
#include <rte_ip.h>

namespace idk::base {

class DpdkMasterServiceImpl {
public:
  struct Args {
    static constexpr std::string_view kBinaryName = "dpdk_master";
    static constexpr std::string_view kHelp =
        "A tool responsible for dpdk initialisation. \n"
        "Initialises all network devices with bound dpdk driver. Check dpdk-devbind.py -s for devices info."
        "Must be launched before any other service using dpdk for network connections \n"
        "Must be active during all other services lifetime.";
    static constexpr std::string_view kVersion = "0.1";

    arg::Argument<std::optional<std::string>, "help for custom_parameter"> config;
  };

  DpdkMasterServiceImpl(Args args, ToolLauncherContext* tool_ctx) : args(std::move(args)), tool_ctx(tool_ctx) {}

  ~DpdkMasterServiceImpl();

  int
  run();

  void
  init();

private:
  struct dpdk_primary_context {
    rte_mempool* mbuf_pools[32];
  };

  Args args;
  ToolLauncherContext* tool_ctx;
  dpdk_primary_context ctx{};

  std::vector<uint16_t> port_ids;
};

using DpdkMasterService = ToolLauncher<DpdkMasterServiceImpl>;

} // namespace idk::base
