file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_macros INTERFACE) # FIXME STATIC
add_library(base::macros ALIAS base_macros)
target_sources(base_macros PUBLIC ${headers})

target_link_libraries(base_macros INTERFACE fmt::fmt)

base_add_options(base_macros INTERFACE)
