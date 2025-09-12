# Dpdk websocket demo

## Disclaimer

This is not a production ready library, nor is it fully optimised.  
Consider this repo as a proof of concept of implementing a custom stack using dpdk.

## Build

#### Install idk build tool
See https://github.com/IDK-Demo/idk

#### Clone via idk
```
idk clone interview_demo
```

#### Build via idk
```
cd interview_demo
idk bundle build
```

## Launch
### Launch dpdk primary process
```bash
sudo .idk/build/Release/bin/network/dpdk_master -v
```

### App
Prepare interface config. Example:
```yaml
gateways:
  - name: default
    ip: 192.168.1.1
    mac: 50:ff:20:21:d6:04

interfaces:
  - name: enp9s0
    pci: 0000:09:00.0
    mac: 52:54:00:e8:24:4f
    ip: 192.168.1.56
    driver: igb_uio
    gateway_mac: 50:ff:20:21:d6:04
```

Prepare application config. Example:
```yaml
cpu_affinity: 9
interface: enp9s0

src_port: 50000
dst_port: 443
dst_ip: 13.113.253.11

ws:
  host: fstream.binance.com
  path: /ws/btcusdt@bookTicker

```

```bash
cd .idk/build/Release/bin/gateway/
sudo ./gateway --config config.yml --interface-config interfaces.yml -v
```
