file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_launcher STATIC)
add_library(base::launcher ALIAS base_launcher)
target_sources(base_launcher PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(base_launcher PUBLIC base::logger base::type cpptrace::cpptrace argparse::argparse)

base_add_options(base_launcher PUBLIC)