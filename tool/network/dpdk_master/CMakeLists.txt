add_executable(dpdk_master main.cpp)
target_link_libraries(dpdk_master base::base network::network dpdk::dpdk)
network_add_options(dpdk_master PRIVATE)
