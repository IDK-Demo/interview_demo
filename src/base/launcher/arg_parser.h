#pragma once

#include <rfl.hpp>
#include <rfl/internal/StringLiteral.hpp>
#include <rfl/json.hpp>

#include "argparse/argparse.hpp"

#include "base/logger/logger.h"
#include "base/type/concept.h"
#include "base/macros/require.h"

namespace idk::base::arg {

template<typename T, rfl::internal::StringLiteral kHelp>
struct Argument;

template<typename T, rfl::internal::StringLiteral kHelp_>
  requires(PrimitiveType<T>)
struct Argument<T, kHelp_> : std::optional<T> {
  using std::optional<T>::optional;
  using Type = T;
  static constexpr rfl::internal::StringLiteral kHelp = kHelp_;
  static constexpr bool kRequired = true;

  operator T() {
    if (!std::optional<T>::has_value()) {
      return {};
    }
    return std::optional<T>::value();
  }
};

template<typename T, rfl::internal::StringLiteral kHelp_>
  requires(!PrimitiveType<T> && !StdOptional<T>)
struct Argument<T, kHelp_> : T {
  using T::T;
  using T::operator=;
  using Type = T;
  static constexpr rfl::internal::StringLiteral kHelp = kHelp_;
  static constexpr bool kRequired = true;

  const T& value() const {
    return static_cast<const T&>(*this);
  }
};

template<typename T, rfl::internal::StringLiteral kHelp_>
  requires(StdOptional<T>)
struct Argument<T, kHelp_> : T {
  using T::T;
  using Type = T;
  static constexpr rfl::internal::StringLiteral kHelp = kHelp_;
  static constexpr bool kRequired = false;
};

namespace impl {

template<typename T>
argparse::Argument&
add_argument(argparse::ArgumentParser& parser, const std::string& name, const std::string& help) {
  if constexpr (std::is_same_v<T, std::string>) {
    return parser.add_argument(name).help(help);
  } else if constexpr (std::is_same_v<T, bool>) {
    return parser.add_argument(name).flag().help(help);
  } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
    return parser.add_argument(name).scan<'i', T>().help(help);
  } else if constexpr (is_std_optional<T>::value) {
    using Inner = typename T::value_type;
    return add_argument<Inner>(parser, name, help);
  } else {
    static_assert(false, "Unsupported field type in CLI parser");
  }
  throw std::runtime_error("Should not reach here");
}

template<typename Params>
void
register_fields(argparse::ArgumentParser& parser, Params& params) {
  // rfl::to_view returns a NamedTuple holding *pointers* to the members
  // of "params"; modifying the view writes back into the original struct.
  auto view = rfl::to_view(params);

  parser.add_argument("-v").help("Debug output").flag().default_value(false);
  parser.add_argument("-vv").help("Trace L1 output").flag().default_value(false);
  parser.add_argument("-vvv").help("Trace L2 output").flag().default_value(false);
  parser.add_argument("-vvvv").help("Trace L3 output").flag().default_value(false);

  view.apply([&](auto field) {
    std::string name = "--" + std::string{decltype(field)::name()};
    for (char& c: name) {
      if (c == '_') {
        c = '-';
      }
    }

    using Arg = std::remove_cvref_t<decltype(*field.value())>;
    std::string help = Arg::kHelp.str();
    using Raw = typename Arg::Type;

    argparse::Argument& argument = add_argument<Raw>(parser, name, help);
    if constexpr (Arg::kRequired) {
      argument.required();
    }
  });
}

// --- second pass: move the parsed values back into the struct --------------

template<typename Params>
void
populate_values(const argparse::ArgumentParser& parser, Params& params) {
  auto view = rfl::to_view(params);

  if (parser.get<bool>("-v")) {
    Logger::set_level(quill::LogLevel::Debug);
  } else if (parser.get<bool>("-vv")) {
    Logger::set_level(quill::LogLevel::TraceL1);
  } else if (parser.get<bool>("-vvv")) {
    Logger::set_level(quill::LogLevel::TraceL2);
  } else if (parser.get<bool>("-vvvv")) {
    Logger::set_level(quill::LogLevel::TraceL3);
  }

  view.apply([&](auto field) {
    std::string name = "--" + std::string{decltype(field)::name()};
    for (char& c: name) {
      if (c == '_') {
        c = '-';
      }
    }

    using Arg = std::remove_cvref_t<decltype(*field.value())>;
    using Raw = typename Arg::Type;
    if (!parser.is_used(name))
      return;

    if constexpr (std::is_same_v<Raw, bool>) {
      *field.value() = parser.get<bool>(name);
    } else if constexpr (std::is_same_v<Raw, std::string>) {
      *field.value() = parser.get<std::string>(name);
    } else if constexpr (std::is_integral_v<Raw> && !std::is_same_v<Raw, bool>) {
      *field.value() = parser.get<Raw>(name);
    } else if constexpr (is_std_optional<Raw>::value) {
      using Inner = typename Raw::value_type;
      *field.value() = parser.get<Inner>(name);
    }
  });
}
} // namespace impl

struct LoggingArgs {
  Argument<std::optional<bool>, "Debug logging level">
  v = false;
  Argument<std::optional<bool>, "Trace logging level">
  vv = false;
};

template<typename Params>
Params
parse(int argc, const char** argv, argparse::ArgumentParser& parser) {
  using namespace std::chrono_literals;
  Params params{};

  impl::register_fields(parser, params);

  parser.add_description(std::string(Params::kHelp));
  parser.add_argument("-h", "--help")
      .action([&](const std::string& s) {
        std::cout << parser;
        std::exit(0);
      })
      .default_value(false)
      .help("shows help message")
      .implicit_value(true)
      .nargs(0);

  try {
    parser.parse_args(argc, argv);
    impl::populate_values(parser, params);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl << std::endl;
    std::cerr.flush();
    std::this_thread::sleep_for(100ms);
    std::cout << parser << std::endl;
    std::exit(1);
  }

  return params;
}

} // namespace idk::base::arg
