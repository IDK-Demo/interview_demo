#pragma once
#include <string>


#include "base/macros/require.h"
#include "base/type/span.h"
#include "device.h"
#include "network/interface/interface_manager.h"
#include "network/type/mac.h"
#include "packet.h"

namespace idk::net {

class Dpdk : base::NoCopy {
public:
  struct DeviceInfo {
    std::string interface_name;
    size_t cpu_affinity;
    uint16_t queue_id{0};
  };
  Dpdk(const InterfaceManager& interface_manager, const std::vector<DeviceInfo>& devices_info);
  Dpdk(Dpdk&& rhs) = default;

  ~Dpdk();

  dpdk::Device&
  get_device(uint16_t port_id);

  dpdk::Device&
  get_device(const std::string& interface_name);

private:
  static std::atomic_flag initialized;
  std::unique_ptr<bool> ownership_flag;
  std::vector<dpdk::Device> devices;
  InterfaceManager interface_manager;
};

} // namespace idk::net
