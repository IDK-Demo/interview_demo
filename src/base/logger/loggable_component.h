#pragma once

#include <string>

#include "quill/Logger.h"
#include "quill/Frontend.h"

#include "logger.h"

namespace idk::base {

class LoggableComponent {
public:
  explicit LoggableComponent(const std::string& name);
  explicit LoggableComponent(Logger::QuillLogger* logger) : logger{logger} {}

  Logger::QuillLogger*
  get_logger() const;

private:
  Logger::QuillLogger* logger;
};

} // namespace idk::base

idk::base::Logger::QuillLogger*
get_logger();
