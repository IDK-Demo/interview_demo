file(GLOB_RECURSE sources "*.cpp")

enable_testing()

add_executable(
    network_test
    ${sources}
)

target_link_libraries(
    network_test
    PRIVATE
    GTest::gtest
    base::base
    network::network
)

include(GoogleTest)
gtest_discover_tests(network_test)

target_add_project_options(network_test PRIVATE base)