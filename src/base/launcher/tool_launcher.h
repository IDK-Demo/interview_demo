#pragma once
#include <filesystem>
#include <iostream>
#include <tuple>
#include <type_traits>

#include <cpptrace/formatting.hpp>
#include <cpptrace/from_current.hpp>

#include "arg_parser.h"
#include "base/launcher/macros.h"
#include "base/logger/logger.h"
#include "base/serialiser/auto/read_file.h"

namespace idk::base {

struct ToolLauncherContext {
  bool
  is_stopped() {
    return stopped.test();
  }

  void
  wait_for_signal() {
    char buf;
    std::ignore = read(signal_pipe_fd[0], &buf, 1);
  }


  static std::atomic_flag stopped;
  static int signal_pipe_fd[2];
};

namespace impl {

template<typename T, typename = void>
struct HasConfig : std::false_type {};

template<typename T>
struct HasConfig<T, std::void_t<typename T::Config>> : std::true_type {};

template<typename T>
inline constexpr bool HasConfig_v = HasConfig<T>::value;

} // namespace impl

template<typename Impl>
class ToolLauncher {
public:
  ToolLauncher(const int argc, const char* argv[]) : argc(argc), argv(argv) {
    using Args = typename Impl::Args;
    argparse::ArgumentParser parser(std::string(Args::kBinaryName), std::string(Args::kVersion),
                                    argparse::default_arguments::none);
    if constexpr (impl::HasConfig_v<Impl>) {
      parser.add_argument("--config").help("Path to configuration file");
    }
    const auto& args = arg::parse<Args>(argc, argv, parser);

    impl.emplace(args, &context);
  }

  int
  run() {
    REQUIRE(!launch_called, "launch was already called");
    launch_called = true;
    std::ignore = pipe(context.signal_pipe_fd);
    signal(SIGINT, interrupt_signal_handler);
    signal(SIGTERM, interrupt_signal_handler);

    CPPTRACE_TRY { return impl->run(); }
    CPPTRACE_CATCH(const std::exception& e) {
      IDK_CATCH_AND_PRINT_STACKTRACE(e);
      return 1;
    }
    return 1;
  }

  void
  stop() {
    ToolLauncherContext::stopped.test_and_set();
  }

  [[nodiscard]] bool
  is_stopped() {
    return ToolLauncherContext::stopped.test();
  }

private:
  static void
  interrupt_signal_handler(int signal) {
    std::string msg = "Signal received: " + std::to_string(signal) + "\n";
    std::ignore = write(STDOUT_FILENO, msg.c_str(), msg.size());
    ToolLauncherContext::stopped.test_and_set();
    char c = 'x';
    std::ignore = write(ToolLauncherContext::signal_pipe_fd[1], &c, 1);
  }

  ToolLauncherContext context;
  const int argc;
  const char** argv;
  bool launch_called = false;
  std::optional<Impl> impl;
};

} // namespace idk::base
