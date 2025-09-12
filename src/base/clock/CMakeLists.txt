file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_clock STATIC)
add_library(base::clock ALIAS base_clock)

target_sources(base_clock PRIVATE ${sources} PUBLIC ${headers})
target_link_libraries(base_clock base::type base::logger)
base_add_options(base_clock PUBLIC)
