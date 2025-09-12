add_library(base_base INTERFACE)
add_library(base::base ALIAS base_base)

add_subdirectory(clock)
target_link_libraries(base_base INTERFACE base::clock)

add_subdirectory(logger)
target_link_libraries(base_base INTERFACE base::logger)

add_subdirectory(macros)
target_link_libraries(base_base INTERFACE base::macros)

add_subdirectory(type)
target_link_libraries(base_base INTERFACE base::type)

add_subdirectory(serialiser)
target_link_libraries(base_base INTERFACE base::serialiser)

add_subdirectory(thread)
target_link_libraries(base_base INTERFACE base::thread)

add_subdirectory(launcher)
target_link_libraries(base_base INTERFACE base::launcher)

add_subdirectory(stream)
target_link_libraries(base_base INTERFACE base::stream)

