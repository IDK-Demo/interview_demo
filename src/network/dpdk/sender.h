#pragma once
#include "base/type/span.h"
#include "packet.h"


namespace idk::net::dpdk {

class Device;

class Sender {
public:
  TxPacket
  get_send_buffer() const;

  void
  send_raw(TxPacket packet, size_t size);

  base::MutableByteView
  send_raw(size_t size);

  void
  flush();

private:
  friend class Device;
  explicit Sender(Device* device);

  Device* device;
};

} // namespace idk::net::dpdk
