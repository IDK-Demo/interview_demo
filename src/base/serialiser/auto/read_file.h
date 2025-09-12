#pragma once

#include <filesystem>

#include "base/serialiser/yaml/yaml.h"
#include "base/serialiser/json/json.h"
#include "base/macros/require.h"

namespace idk::base {

template<typename T>
T parse_file(const std::filesystem::path& path) {
  REQUIRE(exists(path), "File does not exist: {}", path);
  REQUIRE(path.has_extension(), "File extension not found");
  if (path.extension() == ".yaml" || path.extension() == ".yml") {
    return from_yaml_file<T>(path);
  }
  if (path.extension() == ".json") {
    return from_json_file<T>(path);
  }
  REQUIRE(false, "Unknown file extension: {}", path.extension());
}

}
