file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE headers "*.h")

add_library(base_thread STATIC)
add_library(base::thread ALIAS base_thread)
target_sources(base_thread PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(base_thread PUBLIC base::logger)

base_add_options(base_thread PUBLIC)