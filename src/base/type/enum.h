#pragma once
#include <quill/backend/StringFromTime.h>
#include <rfl.hpp>

#include "concept.h"

namespace idk::base {

template<EnumConcept T>
constexpr size_t enum_size() {
  return rfl::get_enumerator_array<T>().size();
}

template<EnumConcept T>
T enum_from_string(const std::string& str) {
  const auto result = rfl::string_to_enum<T>(str);
  REQUIRE(result, "Failed to convert string {} to enum {}", str, typeid(T).name());
  return *result;
}

}
