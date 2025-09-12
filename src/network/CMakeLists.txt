file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(network_network STATIC)
add_library(network::network ALIAS network_network)
message(STATUS "network sources ${sources}")
target_sources(network_network PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(network_network PUBLIC base::base dpdk::dpdk)

network_add_options(network_network PUBLIC)
