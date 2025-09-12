#include "loggable_component.h"

namespace idk::base {

LoggableComponent::LoggableComponent(const std::string& name) {
  logger = Logger::Frontend::create_or_get_logger(name, Logger::get_logger()->get_sinks(),
                                                 Logger::get_logger()->get_pattern_formatter_options(),
                                                 Logger::get_logger()->get_clock_source_type(),
                                                 Logger::get_logger()->get_user_clock_source());
  logger->set_log_level(Logger::get_logger()->get_log_level());
}
Logger::QuillLogger*
LoggableComponent::get_logger() const {
  return logger;
}
} // namespace idk::base

idk::base::Logger::QuillLogger*
get_logger() {
  return idk::base::Logger::get_logger();
}
