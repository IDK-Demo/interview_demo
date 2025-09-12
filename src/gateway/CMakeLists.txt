file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(gateway_gateway STATIC)
add_library(gateway::gateway ALIAS gateway_gateway)
message(STATUS "gateway sources ${sources}")
target_sources(gateway_gateway PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(gateway_gateway PUBLIC base::base network::network dpdk::dpdk)

gateway_add_options(gateway_gateway PUBLIC)
