# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses the `idk` build tool (https://github.com/IDK-Demo/idk), which wraps CMake.

### Build Commands

```bash
# Build the project
idk bundle build

# Build with specific build type (Debug/Release)
idk bundle build --build-type Release

# Run tests
idk bundle run base_test
idk bundle run network_test
```

### Running Tests

The project uses Google Test. To run a single test or test suite:
```bash
# Run all base tests
idk bundle run base_test

# Run specific test (after building)
./idk/build/Debug/bin/base/base_test --gtest_filter=TestName.*
```

## Project Architecture

### Core Components

**`src/base/`** - Foundation library with reusable utilities:
- `clock/` - RDTSC-based high-performance clock implementation
- `logger/` - Logging system with formatters (uses quill and fmt)
- `serialiser/` - JSON/YAML serialization (uses reflect-cpp)
- `thread/` - Thread utilities including CPU affinity
- `launcher/` - Application launcher framework with argument parsing
- `encoding/` - Base64 and other encoding utilities
- `stream/` - Byte stream abstractions
- `type/` - Type utilities and concepts for C++23
- `macros/` - Common macros and compile-time utilities

**`src/network/`** - Network stack implementation:
- `dpdk/` - DPDK integration for kernel-bypass networking (primary/secondary process model)
- `eth_ip/` - Ethernet and IP layer implementations
- `tcp/` - TCP protocol implementation
- `tls12/` - TLS 1.2 client (uses OpenSSL)
- `wss/` - WebSocket Secure client
- `arp/` - ARP protocol handler
- `interface/` - Network interface management
- `type/` - Network types (IP, MAC, port, endpoints, big-endian)

**`tool/`** - Executable binaries:
- `network/dpdk_master/` - DPDK primary process (must run first with sudo)
- `gateway/demo/` - Demo application showing WebSocket over custom network stack

### Application Architecture

This is a **kernel-bypass networking system** using DPDK:

1. **DPDK Primary Process** (`dpdk_master`) must be launched first with sudo
2. **Gateway Application** runs as DPDK secondary process, communicates with primary
3. Network stack is implemented in userspace (Ethernet → IP → TCP → TLS → WebSocket)
4. Configuration uses YAML files for interfaces and application settings

### Dependencies

- **C++23** standard required
- **DPDK** - Data Plane Development Kit for fast packet processing
- **fmt** - String formatting
- **quill** - Logging
- **reflect-cpp** - Reflection-based serialization
- **OpenSSL** - TLS implementation
- **GTest** - Unit testing
- **argparse** - Command-line parsing
- **cpptrace** - Stack traces

## Running Applications

### Launch DPDK Primary Process

```bash
sudo .idk/build/Release/bin/network/dpdk_master -v
```

### Launch Gateway Application

Requires two YAML config files (see README.md for format):
- Interface config: Network interface configuration (PCI address, IP, MAC, gateway)
- App config: Application settings (CPU affinity, WebSocket endpoint)

```bash
cd .idk/build/Release/bin/gateway/
sudo ./gateway --config config.yml --interface-config interfaces.yml -v
```

## Common Patterns

- The codebase uses C++23 features extensively (concepts, ranges, etc.)
- Logging uses structured logging via `base::logger`
- Configuration is done via reflect-cpp for automatic YAML/JSON deserialization
- Components inherit from `LoggableComponent` for integrated logging
- DPDK packet handling uses zero-copy where possible