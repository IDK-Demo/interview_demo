#include "arp_handler.h"

namespace idk::net::arp {

ArpHandler::ArpHandler(Host local_host, dpdk::Sender sender) :
    local_host_(std::move(local_host)), sender_(std::move(sender)) {}

void
ArpHandler::handle_packet(base::MutableByteView raw_packet) {
  PacketView arp_packet(raw_packet);

  if (!arp_packet.is_valid()) {
    TRACE("Ignoring invalid ARP packet");
    return;
  }

  const auto& arp_header = arp_packet.header();
  TRACE_L3("ARP packet: {}", arp_header);

  // Only respond to ARP requests for our IP address
  if (arp_header.operation == Operation::Request && arp_header.target_ip == local_host_.ip) {
    INFO("Responding to ARP request for {} from {}", arp_header.target_ip.as_string(),
         arp_header.sender_ip.as_string());

    // Create ARP reply packet
    auto reply_buffer = sender_.send_raw(PacketView::predict_size());

    PacketView reply_packet(reply_buffer);
    reply_packet.init_reply(local_host_.mac, // sender_mac (our MAC)
                            local_host_.ip, // sender_ip (our IP)
                            arp_header.sender_mac, // target_mac (requester's MAC)
                            arp_header.sender_ip // target_ip (requester's IP)
    );

    sender_.flush();
    TRACE("Sent ARP reply to {}", arp_header.sender_ip.as_string());
  }
}

void
ArpHandler::send_request(Ip target_ip) {
  INFO("Sending ARP request for {}", target_ip.as_string());

  auto request_buffer = sender_.send_raw(PacketView::predict_size());

  PacketView request_packet(request_buffer);

  // Create broadcast ARP request
  Mac broadcast_mac("ff:ff:ff:ff:ff:ff");
  Mac zero_mac("00:00:00:00:00:00");

  // Note: init_request sets Ethernet dst to target_mac parameter, which isn't ideal for broadcast
  // But we can use it and then fix the values we need
  request_packet.init_request(local_host_.mac, // sender_mac
                              local_host_.ip, // sender_ip
                              zero_mac, // target_mac (unknown MAC in ARP header)
                              target_ip // target_ip
  );

  // Fix the Ethernet header to use broadcast (init_request set it to zero_mac)
  request_packet.eth().header().dst_mac = broadcast_mac;

  sender_.flush();
  TRACE("Sent broadcast ARP request for {}", target_ip.as_string());
}

} // namespace idk::net::arp
