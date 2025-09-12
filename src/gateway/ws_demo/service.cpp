#include "service.h"

#include "base/thread/cpu.h"
#include "network/interface/interface_manager.h"
#include "network/wss/client.h"

namespace idk {

WsDemoServiceImpl::WsDemoServiceImpl(Args args_, base::ToolLauncherContext* ctx) :
    args(std::move(args_)), ctx(ctx), interface_manager(args.interface_config) {}

int
WsDemoServiceImpl::run() {
  auto config = base::parse_file<Config>(args.config.value());
  base::Cpu::bind_this_thread_to_cpu(config.cpu_affinity);

  interface = interface_manager.get_interface(config.interface);
  dpdk.emplace(net::Dpdk{interface_manager, {{config.interface, config.cpu_affinity}}});
  auto* device = &dpdk->get_device(config.interface);
  std::ignore = device->clear_receive_queue();

  arp_handler.emplace(net::Host{.mac = interface.mac, .ip = interface.ip}, device->get_sender());

  net::tcp::Client tcp(
     net::Connection{
         .session = {.src = {.mac = interface.mac, .ip = interface.ip},
                     .dst = {.mac = interface.gateway_mac, .ip = config.dst_ip}},
         .src_port = net::Port(config.src_port),
         .dst_port = net::Port(config.dst_port),
     },
     device->get_sender());

  connection.emplace(config.ws, std::move(tcp));

  while (!ctx->is_stopped()) {
    const auto raw_packet = device->receive();
    if (raw_packet.has_value()) {
      net::EthernetPacketView eth(raw_packet->bytes());
      TRACE("Eth packet of ARP: {} valid: {} header: {}", eth.header().type == net::EthernetType::Arp, eth.is_valid(),
            eth.header());
      if (eth.header().type == net::EthernetType::Arp) {
        arp_handler->handle_packet(raw_packet->bytes());
        continue;
      }
      if (eth.header().type != net::EthernetType::Ipv4) {
        continue;
      }

      net::IPPacketView ip(eth);
      if (ip.header().protocol != net::IpProtocol::Tcp) {
        continue;
      }

      net::tcp::PacketView tcp(ip);

      connection->process_packet(tcp);
      auto payload = connection->next_message();
      while (payload) {
        process_payload(payload.value());
        payload = connection->next_message();
      }
      TRACE("No payload. Going to next packet");
    }
  }

  return 0;
}

void
WsDemoServiceImpl::process_payload(base::ByteView payload) {
  DEBUG("Ws msg: {}", base::to_string_view(payload));
}


} // namespace idk
