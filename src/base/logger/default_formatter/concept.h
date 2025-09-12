#pragma once

#include <type_traits>

namespace idk::base {

template<typename E>
concept ScopedEnumConcept =
    std::is_enum_v<E> &&
   !std::is_convertible_v<E, std::underlying_type_t<E>>;

template<typename T>
concept LoggableConcept = (requires {
  { T::kLoggable } -> std::convertible_to<int>;
} && T::kLoggable == true);

template<typename T>
concept FastLoggableConcept = requires {
  { T::kFastLoggable } -> std::convertible_to<bool>;
} && T::kFastLoggable == true;

}
