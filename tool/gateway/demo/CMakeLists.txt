add_executable(gateway main.cpp)
target_link_libraries(gateway gateway::gateway dpdk::dpdk)
gateway_add_options(gateway PRIVATE)
