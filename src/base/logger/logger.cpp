#include "logger.h"

#include <pthread.h>
#include <ranges>
#include <utility>

#include "quill/sinks/RotatingFileSink.h"

using namespace std::chrono;
using namespace std::chrono_literals;

namespace idk::base {

std::optional<Logger::Params> Logger::params_{};
std::mutex Logger::mutex{};

Logger::Logger(Params params) {
  std::lock_guard lock(mutex);

  quill::BackendOptions backend_options;
  backend_options.log_level_short_codes = {"TR3", "TR2", "TR1", "DBG", "INF", "NTC",
                                           "WRN", "ERR", "CRT", "BTC", "___", "DNC"};
  backend_options.thread_name = "logger";
  backend_options.rdtsc_resync_interval = 100ms;
  backend_options.cpu_affinity = 0;
  
  quill::detail::set_thread_name("main");
  quill::PatternFormatterOptions formatter{
      "%(time) %(log_level_short_code) %(message:<100) [%(logger)] [%(thread_name)] %(short_source_location)",
      "%H:%M:%S.%Qns", quill::Timezone::GmtTime};

  auto console_formatter = formatter;
  console_formatter.format_pattern = "\r" + console_formatter.format_pattern;
  auto console = Frontend::create_or_get_sink<quill::ConsoleSink>("console");

  std::shared_ptr<quill::Sink> sink;
  if (params.logfile.has_value() && params.rotate) {
    auto file_sink = Frontend::create_or_get_sink<quill::RotatingFileSink>(
        params.logfile.value(),
        [] {
          quill::RotatingFileSinkConfig cfg;

          /* --- file/open options (inherited from FileSink) --- */
          cfg.set_open_mode('a'); // 'a' = append, 'w' = truncate
          cfg.set_filename_append_option(quill::FilenameAppendOption::StartDate); // app.log → app_20250429.log

          /* --- rotation triggers --------------------------- */
          cfg.set_rotation_max_file_size(10 * 1024 * 1024); // 10 MB
          cfg.set_rotation_frequency_and_interval('H', 1); // every hour
          // OR cfg.set_rotation_time_daily("02:00");                // every day 02:00

          /* --- retention / naming -------------------------- */
          cfg.set_max_backup_files(7); // keep 7 old files
          cfg.set_rotation_naming_scheme(quill::RotatingFileSinkConfig::RotationNamingScheme::Index);
          // Index scheme:          app.log, app.1.log, app.2.log …
          // Date scheme:           app.20250429.log
          // DateAndTime scheme:    app.20250429_023015.log

          return cfg;
        }(),
        quill::FileEventNotifier{});
  } else if (params.logfile.has_value()) {
    sink = Frontend::create_or_get_sink<quill::FileSink>(
        params.logfile.value(),
        [] {
          quill::FileSinkConfig cfg;
          cfg.set_open_mode('w');
          return cfg;
        }(),
        quill::FileEventNotifier{});
  } else {
    formatter = console_formatter;
    sink = console;
  }

  auto clock_source = quill::ClockSourceType::Tsc;
  quill::UserClockSource* clock = nullptr;


  auto system_logger = Frontend::create_or_get_logger(kSystemLogger, console, console_formatter);
  Frontend::create_or_get_logger(kDefaultLogger, std::move(sink), formatter, clock_source, clock);

  backend_options.error_notifier = [system_logger](const std::string& message) {
    LOG_WARNING(system_logger, "{}", message);
  };

  quill::Backend::start(backend_options);
  params_ = params;
}

void
Logger::set_level(Level level) {
  get_logger()->set_log_level(level);
  for (auto* logger: Frontend::get_all_loggers()) {
    if (logger) {
      logger->set_log_level(level);
    }
  }
}

Logger::Level
Logger::get_level() {
  return get_logger()->get_log_level();
}

Logger::Level
Logger::parse_log_level(const std::string& s) {
  std::ranges::transform_view to_lower(s, [](char c) { return std::tolower(c); });
  std::string lower(to_lower.begin(), to_lower.end());
  if (lower == "trace_l3")
    return Level::TraceL3;
  if (lower == "trace_l2")
    return Level::TraceL2;
  if (lower == "trace_l1")
    return Level::TraceL1;
  if (lower == "debug")
    return Level::Debug;
  if (lower == "info")
    return Level::Info;
  if (lower == "warn" || lower == "warning")
    return Level::Warning;
  if (lower == "notice")
    return Level::Notice;
  if (lower == "error")
    return Level::Error;
  if (lower == "critical")
    return Level::Critical;
  throw std::invalid_argument("Invalid log level: " + s);
}

Logger::QuillLogger*
Logger::get_logger() {
  return Frontend::get_logger(kDefaultLogger);
}

Logger::QuillLogger*
Logger::get_system_logger() {
  return Frontend::get_logger(kSystemLogger);
}
void
Logger::remove_logger(QuillLogger* logger) {
  Frontend::remove_logger_blocking(logger);
}

Logger::Params
Logger::get_params() {
  std::lock_guard lock(mutex);
  if (!params_.has_value()) {
    throw std::runtime_error("Logger is not initialised.");
  }
  return params_.value();
}

Logger::QuillLogger*
Logger::get_logger(const std::string& component_name) {
  auto* logger = Frontend::create_or_get_logger(component_name, get_logger()->get_sinks(),
                                                 get_logger()->get_pattern_formatter_options(),
                                                 get_logger()->get_clock_source_type(),
                                                 get_logger()->get_user_clock_source());
  logger->set_log_level(get_logger()->get_log_level());
  return logger;
}


Logger::~Logger() { quill::Backend::stop(); }

} // namespace idk::base
