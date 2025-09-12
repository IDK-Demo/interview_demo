#pragma once
#include <concepts>
#include <optional>

namespace idk::base {

template <typename T>
struct is_std_optional : std::false_type {};

template <typename U>
struct is_std_optional<std::optional<U>> : std::true_type {};

template<typename T>
concept PrimitiveType = std::is_fundamental_v<T>;

template<typename T>
concept StdOptional = is_std_optional<T>::value;

template<typename E>
concept EnumConcept =
    std::is_enum_v<E> &&
   !std::is_convertible_v<E, std::underlying_type_t<E>>;

}
