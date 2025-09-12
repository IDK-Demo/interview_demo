#pragma once

#include <iostream>

#include <fmt/format.h>

#include "base/logger/logger.h"

#define REQUIRE(statement, message, ...)                                                                               \
  do {                                                                                                                 \
    if (!(statement)) [[unlikely]] {                                                                                   \
      ERROR(message, ##__VA_ARGS__);                                                                                   \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)


#define REQUIRE_EQ(a, b, message, ...)                                                                                 \
  do {                                                                                                                 \
    if ((a) != (b)) [[unlikely]] {                                                                                     \
      ERROR("{} != {}  {}", a, b, fmt::format(message, ##__VA_ARGS__));                                                \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)

#define REQUIRE_NE(a, b, message, ...)                                                                                 \
  do {                                                                                                                 \
    if ((a) == (b)) [[unlikely]] {                                                                                     \
      ERROR("{} != {}  {}", a, b, fmt::format(message, ##__VA_ARGS__));                                                \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)

#define REQUIRE_LE(a, b, message, ...)                                                                                 \
  do {                                                                                                                 \
    if ((a) > (b)) [[unlikely]] {                                                                                      \
      ERROR("{} > {}  {}", a, b, fmt::format(message, ##__VA_ARGS__));                                                 \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)

#define REQUIRE_LT(a, b, message, ...)                                                                                 \
  do {                                                                                                                 \
    if ((a) >= (b)) [[unlikely]] {                                                                                     \
      ERROR("{} >= {}  {}", a, b, fmt::format(message, ##__VA_ARGS__));                                                \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)

#define REQUIRE_GE(a, b, message, ...)                                                                                 \
  do {                                                                                                                 \
    if ((a) < (b)) [[unlikely]] {                                                                                      \
      ERROR("{} < {}  {}", a, b, fmt::format(message, ##__VA_ARGS__));                                                 \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)

#define REQUIRE_GT(a, b, message, ...)                                                                                 \
  do {                                                                                                                 \
    if ((a) <= (b)) [[unlikely]] {                                                                                     \
      ERROR("{} <= {}  {}", a, b, fmt::format(message, ##__VA_ARGS__));                                                \
      throw std::runtime_error(fmt::format(message, ##__VA_ARGS__));                                                   \
    }                                                                                                                  \
  } while (false)
