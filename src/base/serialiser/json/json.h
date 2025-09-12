#pragma once

#include <string>
#include <chrono>

#include <rfl.hpp>
#include <rfl/json.hpp>
#include "base/logger/rfl_chrono_crutch/_chrono_parser.h"
#include "base/macros/require.h"

namespace idk::base {

template<typename T>
T from_json_string(const std::string& s) {
  return rfl::json::read<T, rfl::DefaultIfMissing, rfl::NoExtraFields>(s).value();
}

// Version that allows extra fields (useful for external APIs)
template<typename T>
T from_json_string_lenient(const std::string& s) {
  return rfl::json::read<T, rfl::DefaultIfMissing>(s).value();
}

template<typename T>
T from_json_file(const std::filesystem::path& path) {
  REQUIRE(exists(path), "File does not exist: {}", path);
  std::ifstream file(path, std::ios::binary); // binary mode prevents newline translations
  REQUIRE(file, "Could not open file: {}", path);

  std::string content;
  content.resize(std::filesystem::file_size(path)); // fast allocation
  file.read(content.data(), content.size());

  return from_json_string<T>(content);
}

template<typename T>
std::string to_json_string(const T& o) {
  return rfl::json::write(o);
}

}
