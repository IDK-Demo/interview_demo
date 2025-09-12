#pragma once
#include "fmt/format.h"
#include "loggable_component.h"
#include "logger.h"

namespace idk::base {

#define TRACE_L3(format_str, ...) LOG_TRACE_L3(get_logger(), format_str, ##__VA_ARGS__)
#define TRACE_L2(format_str, ...) LOG_TRACE_L2(get_logger(), format_str, ##__VA_ARGS__)
#define TRACE_L1(format_str, ...) LOG_TRACE_L1(get_logger(), format_str, ##__VA_ARGS__)
#define DEBUG(format_str, ...) LOG_DEBUG(get_logger(), format_str, ##__VA_ARGS__)
#define INFO(format_str, ...) LOG_INFO(get_logger(), format_str, ##__VA_ARGS__)
#define NOTICE(format_str, ...) LOG_NOTICE(get_logger(), format_str, ##__VA_ARGS__)
#define WARNING(format_str, ...) LOG_WARNING(get_logger(), format_str, ##__VA_ARGS__)
#define ERROR(format_str, ...) LOG_ERROR(get_logger(), format_str, ##__VA_ARGS__)
#define CRITICAL(format_str, ...) LOG_CRITICAL(get_logger(), format_str, ##__VA_ARGS__)
#define BACKTRACE(format_str, ...) LOG_BACKTRACE(get_logger(), format_str, ##__VA_ARGS__)

// Throttled
#define TRACE_L3_LIMIT(min_interval, format_str, ...)                                                                  \
  LOG_TRACE_L3_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define TRACE_L2_LIMIT(min_interval, format_str, ...)                                                                  \
  LOG_TRACE_L2_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define TRACE_L1_LIMIT(min_interval, format_str, ...)                                                                  \
  LOG_TRACE_L1_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define DEBUG_LIMIT(min_interval, format_str, ...)                                                                     \
  LOG_DEBUG_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define INFO_LIMIT(min_interval, format_str, ...) LOG_INFO_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define NOTICE_LIMIT(min_interval, format_str, ...)                                                                    \
  LOG_NOTICE_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define WARN_LIMIT(min_interval, format_str, ...)                                                                      \
  LOG_WARNING_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define ERROR_LIMIT(min_interval, format_str, ...)                                                                     \
  LOG_ERROR_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)
#define CRITICAL_LIMIT(min_interval, format_str, ...)                                                                  \
  LOG_CRITICAL_LIMIT(min_interval, get_logger(), format_str, ##__VA_ARGS__)

// Every N occurrences
#define TRACE_L3_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                         \
  LOG_TRACE_L3_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define TRACE_L2_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                         \
  LOG_TRACE_L2_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define TRACE_L1_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                         \
  LOG_TRACE_L1_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define DEBUG_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                            \
  LOG_DEBUG_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define INFO_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                             \
  LOG_INFO_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define NOTICE_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                           \
  LOG_NOTICE_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define WARNING_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                          \
  LOG_WARNING_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define ERROR_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                            \
  LOG_ERROR_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)
#define CRITICAL_LIMIT_EVERY_N(n_occurrences, format_str, ...)                                                         \
  LOG_CRITICAL_LIMIT_EVERY_N(n_occurrences, get_logger(), format_str, ##__VA_ARGS__)

// With tags
#define TRACE_L3_TAGS(tags, format_str, ...) LOG_TRACE_L3_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define TRACE_L2_TAGS(tags, format_str, ...) LOG_TRACE_L2_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define TRACE_L1_TAGS(tags, format_str, ...) LOG_TRACE_L1_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define DEBUG_TAGS(tags, format_str, ...) LOG_DEBUG_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define INFO_TAGS(tags, format_str, ...) LOG_INFO_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define NOTICE_TAGS(tags, format_str, ...) LOG_NOTICE_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define WARNING_TAGS(tags, format_str, ...) LOG_WARNING_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define ERROR_TAGS(tags, format_str, ...) LOG_ERROR_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)
#define CRITICAL_TAGS(tags, format_str, ...) LOG_CRITICAL_TAGS(get_logger(), tags, format_str, ##__VA_ARGS__)

// TODO remove deprecated macros
#define TRACE(format, ...) TRACE_L1(format, ##__VA_ARGS__)
#define WARN(format, ...) WARNING(format, ##__VA_ARGS__)

#define TRACE_THROTTLED(interval, format, ...) TRACE_L1_LIMIT(interval, format, ##__VA_ARGS__)
#define DEBUG_THROTTLED(interval, format, ...) DEBUG_LIMIT(interval, format, ##__VA_ARGS__)
#define INFO_THROTTLED(interval, format, ...) INFO_LIMIT(interval, format, ##__VA_ARGS__)
#define WARN_THROTTLED(interval, format, ...) WARN_LIMIT(interval, format, ##__VA_ARGS__)
#define ERROR_THROTTLED(interval, format, ...) ERROR_LIMIT(interval, format, ##__VA_ARGS__)
} // namespace idk::base
