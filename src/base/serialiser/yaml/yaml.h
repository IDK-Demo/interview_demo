#pragma once

#include <string>
#include <chrono>

#include <rfl.hpp>
#include <rfl/yaml.hpp>
#include "base/logger/rfl_chrono_crutch/_chrono_parser.h"
#include "base/macros/require.h"

namespace idk::base {

template<typename T>
T from_yaml_string(const std::string& s) {
  return rfl::yaml::read<T, rfl::DefaultIfMissing, rfl::NoExtraFields>(s).value();
}

template<typename T>
T from_yaml_file(const std::filesystem::path& path) {
  REQUIRE(exists(path), "File does not exist: {}", path);
  std::ifstream file(path, std::ios::binary); // binary mode prevents newline translations
  REQUIRE(file, "Could not open file: {}", path);

  std::string content;
  content.resize(std::filesystem::file_size(path)); // fast allocation
  file.read(content.data(), content.size());

  return from_yaml_string<T>(content);
}

template<typename T>
std::string to_yaml_string(const T& o) {
  return rfl::yaml::write(o);
}

}
