#pragma once

#include <mutex>
#include <optional>
#include <string>

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h"

#include "default_formatter/default_formatters.h"

namespace idk::base {

class Logger {
public:
  using Level = quill::LogLevel;
  struct Params {
    std::optional<std::string> logfile;
    std::optional<std::string> metrics_root;
    bool rotate = false;
  };
  Logger(Params params = Params{.rotate = false});

  struct FrontendOptions {
    static constexpr quill::QueueType queue_type = quill::QueueType::UnboundedBlocking;
    static constexpr size_t initial_queue_capacity = 128u * 1024u * 1024u; // 128 MiB
    static constexpr uint32_t blocking_queue_retry_interval_ns = 800;
    static constexpr size_t unbounded_queue_max_capacity = 2ull * 1024u * 1024u * 1024u; // 2 GiB
    static constexpr quill::HugePagesPolicy huge_pages_policy = quill::HugePagesPolicy::Never;
  };

  using Frontend = quill::FrontendImpl<FrontendOptions>;
  using QuillLogger = Frontend::logger_t;
  ~Logger();

  static void
  set_level(Level level);

  static Level
  get_level();

  static Level
  parse_log_level(const std::string& s);

  static QuillLogger*
  get_logger();

  static QuillLogger*
  get_logger(const std::string& component_name);

  static QuillLogger*
  get_system_logger();

  static void
  remove_logger(QuillLogger* logger);

  static Params
  get_params();

private:
  static constexpr const char* kDefaultLogger = "default";
  static constexpr const char* kSystemLogger = "system";

  static std::optional<Params> params_;
  static std::mutex mutex;
};

} // namespace idk::base

#include "macros.h"
