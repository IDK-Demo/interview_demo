#include "dpdk.h"
#include <rte_version.h>

#include "rte_eal.h"
#include "rte_ethdev.h"
#include "rte_ip.h"

#include "network/interface/interface_manager.h"
#include "base/thread/execute_with_timeout.h"

using namespace std::chrono_literals;

namespace idk::net {

namespace impl {

std::string
rte_eal_init_error(int code) {
  switch (code) {
    case EACCES:
      return "Permissions issue";
    case EAGAIN:
      return "Bus or system resource was not available, setup may be attempted again";
    case EALREADY:
      return "rte_eal_init function has already been called";
    case EFAULT:
      return "Tailq configuration name was not found in memory configuration";
    case EINVAL:
      return "Invalid parameters were passed as argv/argc";
    case ENOMEM:
      return "Out-of-memory condition";
    case ENODEV:
      return "Memory setup issues";
    case ENOTSUP:
      return "EAL cannot initialize on this system";
    case EPROTO:
      return "PCI bus is either not present or not readable by the EAL";
    case ENOEXEC:
      return "Service core failed to launch successfully";
    default:
      return "Unknown error code: " + std::to_string(code);
  }
}

uint16_t
dpdk_port_by_pci_addr(const std::string& pci_addr) {
  uint16_t port_id;
  char dev_name[RTE_ETH_NAME_MAX_LEN];
  INFO("Looking for device with PCI address: {}", pci_addr);
  RTE_ETH_FOREACH_DEV(port_id) {
    if (rte_eth_dev_get_name_by_port(port_id, dev_name) != 0) {
      DEBUG("Unable to get device name for port {}, skip to next port", port_id);
      continue;
    }
    INFO("Port {}: device name = {}", port_id, dev_name);

    if (std::strstr(dev_name, pci_addr.c_str()) != nullptr) {
      // Check if port is valid before returning
      if (!rte_eth_dev_is_valid_port(port_id)) {
        ERROR("Port {} for device {} is not valid!", port_id, pci_addr);
        continue;
      }
      INFO("Found port {} for PCI address {}", port_id, pci_addr);
      return port_id;
    }
  }
  REQUIRE(false, "Unable to find device with name {}", pci_addr);
}

} // namespace impl

std::atomic_flag Dpdk::initialized(ATOMIC_FLAG_INIT);

Dpdk::Dpdk(const InterfaceManager& interface_manager, const std::vector<DeviceInfo>& devices_info) : interface_manager(interface_manager) {
  INFO("Initializing DPDK");
  base::execute_with_timeout(
      2s,
      [&] {
        REQUIRE(!initialized.test_and_set(), "DPDK is already initialized");
        ownership_flag = std::make_unique<bool>(true);
        REQUIRE(!devices_info.empty(), "No devices provided");
        INFO("Initializing DPDK of version {}", rte_version());
        std::string cpu_str;
        for (const auto& [_, cpu_id, _1]: devices_info) {
          cpu_str += std::to_string(cpu_id) + ",";
        }
        if (!cpu_str.empty()) {
          cpu_str.pop_back();
        }
        const char* eal_args[] = {
            "name",
            "-l",
            cpu_str.c_str(),
            "--proc-type=secondary",
        };
        INFO("cput_str: {}", cpu_str);
        const int eal_argc = std::size(eal_args);
        REQUIRE(rte_eal_init(eal_argc, const_cast<char**>(eal_args)) == eal_argc - 1, "Failed to initialize EAL: {}",
                impl::rte_eal_init_error(rte_errno));

        for (const auto& [interface_name, _, queue_id]: devices_info) {
          const auto pci_addr = interface_manager.get_interface(interface_name).pci;
          INFO("Initializing DPDK device: {}", pci_addr);
          auto port_id = impl::dpdk_port_by_pci_addr(pci_addr);
          INFO("Device {} is on port {}", pci_addr, port_id);
          REQUIRE(rte_eth_dev_is_valid_port(port_id), "Device {} is not attached", pci_addr);
          devices.emplace_back(pci_addr, port_id, interface_name, queue_id);
        }
      },
      "DPDK failed to initialize. Are you launching under sudo?");
}

dpdk::Device&
Dpdk::get_device(uint16_t port_id) {
  for (auto& device: devices) {
    if (device.port_id() == port_id) {
      return device;
    }
  }
  REQUIRE(false, "Unable to find device with port id {}", port_id);
}

dpdk::Device&
Dpdk::get_device(const std::string& interface_name) {
  for (auto& device: devices) {
    if (device.interface_name() == interface_name) {
      return device;
    }
  }
  REQUIRE(false, "Unable to find device {}", interface_name);
}

Dpdk::~Dpdk() {
  if (ownership_flag) {
    rte_eal_cleanup();
  }
}

} // namespace idk::base
