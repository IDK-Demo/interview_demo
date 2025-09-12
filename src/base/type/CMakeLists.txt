file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_type INTERFACE)
add_library(base::type ALIAS base_type)

target_sources(base_type PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(base_type INTERFACE base::logger base::serialiser reflectcpp::reflectcpp)

base_add_options(base_type INTERFACE)
