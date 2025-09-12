file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_serialiser STATIC)
add_library(base::serialiser ALIAS base_serialiser)

target_sources(base_serialiser PUBLIC ${headers} PRIVATE ${sources})
target_link_libraries(base_serialiser PUBLIC base::macros base::type)
base_add_options(base_serialiser PUBLIC)
