#include "sender.h"

#include "base/macros/require.h"
#include "network/dpdk/device.h"

namespace idk::net::dpdk {

Sender::Sender(Device* device) : device(device) {}

base::MutableByteView
Sender::send_raw(size_t size) {
  return device->enqueue_for_send(size);
}

TxPacket
Sender::get_send_buffer() const {
  return device->get_send_buffer();
}

void
Sender::send_raw(TxPacket packet, size_t size) {
  return device->enqueue_for_send(std::move(packet), size);
}

void
Sender::flush() {
  device->flush_send_queue();
}


} // namespace idk::net::dpdk
