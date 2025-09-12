file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_stream STATIC)
add_library(base::stream ALIAS base_stream)
target_sources(base_stream PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(base_stream PUBLIC base::logger)
base_add_options(base_stream PUBLIC)
