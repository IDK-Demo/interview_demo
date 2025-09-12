#include <compare>
#include <cstdint>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "rfl/Result.hpp"
#include "rfl/Tuple.hpp"
#include "rfl/internal/StringLiteral.hpp"
#include "rfl/internal/no_duplicate_field_names.hpp"

namespace rfl {

template <internal::StringLiteral _name>
struct LiteralWithDefaultHelper {
  constexpr static internal::StringLiteral name_ = _name;
};

template <internal::StringLiteral... fields_>
class LiteralWithDefault {
  using FieldsType = rfl::Tuple<LiteralWithDefaultHelper<fields_>...>;

 public:
  LiteralWithDefault() : LiteralWithDefault<fields_...>("nanoseconds") {}

  using ValueType = std::conditional_t<sizeof...(fields_) <= 255, std::uint8_t,
                                       std::uint16_t>;

  /// The number of different fields or different options that the literal
  /// can assume.
  static constexpr ValueType num_fields_ = sizeof...(fields_);

  using ReflectionType = std::string;

  /// Constructs a LiteralWithDefault from another literal.
  LiteralWithDefault(const LiteralWithDefault<fields_...>& _other) = default;

  /// Constructs a LiteralWithDefault from another literal.
  LiteralWithDefault(LiteralWithDefault<fields_...>&& _other) noexcept = default;

  LiteralWithDefault(const std::string& _str) : value_(find_value(_str).value()) {}

  /// A single-field literal is special because it
  /// can also have a default constructor.
  template <ValueType num_fields = num_fields_,
            typename = std::enable_if_t<num_fields <= 1>>
  LiteralWithDefault() : value_(0) {}

  ~LiteralWithDefault() = default;

  /// Constructs a new LiteralWithDefault.
  template <internal::StringLiteral _name>
  static LiteralWithDefault<fields_...> make() {
    return LiteralWithDefault(LiteralWithDefault<fields_...>::template value_of<_name>());
  }

  /// Constructs a new LiteralWithDefault, equivalent to make, for reasons of consistency.
  template <internal::StringLiteral _name>
  static LiteralWithDefault<fields_...> from_name() {
    return LiteralWithDefault<fields_...>::template make<_name>();
  }

  /// Constructs a new LiteralWithDefault.
  template <ValueType _value>
  static LiteralWithDefault<fields_...> from_value() {
    static_assert(_value < num_fields_,
                  "Value cannot exceed number of fields.");
    return LiteralWithDefault<fields_...>(_value);
  }

  /// Constructs a new LiteralWithDefault.
  static Result<LiteralWithDefault<fields_...>> from_value(ValueType _value) {
    if (_value >= num_fields_) {
      return error("Value cannot exceed number of fields.");
    }
    return LiteralWithDefault<fields_...>(_value);
  }

  /// Determines whether the literal contains the string.
  static bool contains(const std::string& _str) {
    bool found = false;
    has_value(_str, &found);
    return found;
  }

  /// Determines whether the literal contains the string at compile time.
  template <internal::StringLiteral _name>
  static constexpr bool contains() {
    return find_value_of<_name>() != -1;
  }

  /// Determines whether the literal contains any of the strings in the other
  /// literal at compile time.
  template <class OtherLiteralType, int _i = 0>
  static constexpr bool contains_any() {
    if constexpr (_i == num_fields_) {
      return false;
    } else {
      constexpr auto name = find_name_within_own_fields<_i>();
      return OtherLiteralType::template contains<name>() ||
             contains_any<OtherLiteralType, _i + 1>();
    }
  }

  /// Determines whether the literal contains all of the strings in the other
  /// literal at compile time.
  template <class OtherLiteralType, int _i = 0, int _n_found = 0>
  static constexpr bool contains_all() {
    if constexpr (_i == num_fields_) {
      return _n_found == OtherLiteralType::num_fields_;
    } else {
      constexpr auto name = find_name_within_own_fields<_i>();
      if constexpr (OtherLiteralType::template contains<name>()) {
        return contains_all<OtherLiteralType, _i + 1, _n_found + 1>();
      } else {
        return contains_all<OtherLiteralType, _i + 1, _n_found>();
      }
    }
  }

  /// Determines whether the literal has duplicate strings at compile time.
  /// These is useful for checking collections of strings in other contexts.
  static constexpr bool has_duplicates() {
    return !internal::no_duplicate_field_names<FieldsType>();
  }

  /// Constructs a LiteralWithDefault from a string. Returns an error if the string
  /// cannot be found.
  static Result<LiteralWithDefault> from_string(const std::string& _str) {
    const auto to_literal = [](const auto& _v) {
      return LiteralWithDefault<fields_...>(_v);
    };
    return find_value(_str).transform(to_literal);
  };

  /// The name defined by the LiteralWithDefault.
  std::string name() const { return find_name(); }

  /// Returns all possible values of the literal as a std::vector<std::string>.
  static std::vector<std::string> names() {
    return allowed_strings_vec(std::make_integer_sequence<int, num_fields_>());
  }

  /// Helper function to retrieve a name at compile time.
  template <int _value>
  constexpr static auto name_of() {
    constexpr auto name = find_name_within_own_fields<_value>();
    return LiteralWithDefault<name>();
  }

  /// Assigns from another literal.
  LiteralWithDefault<fields_...>& operator=(const LiteralWithDefault<fields_...>& _other) = default;

  /// Assigns from another literal.
  LiteralWithDefault<fields_...>& operator=(LiteralWithDefault<fields_...>&& _other) noexcept =
      default;

  /// Assigns the literal from a string
  LiteralWithDefault<fields_...>& operator=(const std::string& _str) {
    value_ = find_value(_str).value();
    return *this;
  }

  /// <=> for other Literals with the same fields.
  auto operator<=>(const LiteralWithDefault<fields_...>& _other) const {
    return value() <=> _other.value();
  }

  /// <=> for other Literals with different fields.
  template <internal::StringLiteral... _fields>
  inline auto operator<=>(const LiteralWithDefault<_fields...>& _l2) const {
    return name() <=> _l2.name();
  }

  /// <=> for strings.
  inline auto operator<=>(const std::string& _str) const {
#if __cpp_lib_three_way_comparison >= 201907L
    return name() <=> _str;
#else
    auto const& const_name = name();
    if (const_name < _str) {
      return std::strong_ordering::less;
    }
    if (const_name == _str) {
      return std::strong_ordering::equal;
    }
    return std::strong_ordering::greater;
#endif
  }

  /// <=> for const char*.
  template <internal::StringLiteral... other_fields>
  inline auto operator<=>(const char* _str) const {
#if __cpp_lib_three_way_comparison >= 201907L
    return name() <=> _str;
#else
    auto const& const_name = name();
    if (const_name < _str) {
      return std::strong_ordering::less;
    }
    if (const_name == _str) {
      return std::strong_ordering::equal;
    }
    return std::strong_ordering::greater;
#endif
  }

  /// Equality operator.
  template <class Other>
  bool operator==(const Other& _other) const {
    return (*this <=> _other) == 0;
  }

  /// Alias for .name().
  std::string reflection() const { return name(); }

  /// Returns the number of fields in the LiteralWithDefault.
  static constexpr size_t size() { return num_fields_; }

  /// Alias for .name().
  std::string str() const { return name(); }

  /// Alias for .names().
  static std::vector<std::string> strings() {
    return allowed_strings_vec(std::make_integer_sequence<int, num_fields_>());
  }

  /// Returns the value actually contained in the LiteralWithDefault.
  ValueType value() const { return value_; }

  /// Returns the value of the string literal in the template.
  template <internal::StringLiteral _name>
  static constexpr ValueType value_of() {
    constexpr auto value = find_value_of<_name>();
    static_assert(value >= 0, "String not supported.");
    return value;
  }

 private:
  /// Only the static methods are allowed to access this.
  LiteralWithDefault(const ValueType _value) : value_(_value) {}

  /// Returns all of the allowed fields.
  static std::string allowed_strings() {
    const auto vec =
        allowed_strings_vec(std::make_integer_sequence<int, num_fields_>());
    std::string str;
    for (size_t i = 0; i < vec.size(); ++i) {
      const auto head = "'" + vec[i] + "'";
      str += i == 0 ? head : (", " + head);
    }
    return str;
  }

  /// Returns all of the allowed fields.
  template <int... _is>
  static std::vector<std::string> allowed_strings_vec(
      std::integer_sequence<int, _is...>) {
    std::vector<std::string> values;
    (allowed_strings_vec_add_one<_is>(&values), ...);
    return values;
  }

  template <int _i>
  static void allowed_strings_vec_add_one(std::vector<std::string>* _values) {
    using FieldType = tuple_element_t<_i, FieldsType>;
    _values->emplace_back(FieldType::name_.str());
  }

  /// Finds the correct index associated with
  /// the string at run time.
  std::string find_name() const {
    return find_name_set_str(std::make_integer_sequence<int, num_fields_>());
  }

  template <int... _is>
  std::string find_name_set_str(std::integer_sequence<int, _is...>) const {
    std::string name;
    (find_name_set_if_matches<_is>(&name), ...);
    return name;
  }

  template <int _i>
  void find_name_set_if_matches(std::string* _name) const {
    if (_i == value_) {
      using FieldType = tuple_element_t<_i, FieldsType>;
      *_name = FieldType::name_.str();
    }
  }

  /// Finds the correct index associated with
  /// the string at compile time within the LiteralWithDefault's own fields.
  template <int _i>
  constexpr static auto find_name_within_own_fields() {
    return tuple_element_t<_i, FieldsType>::name_;
  }

  /// Finds the correct value associated with
  /// the string at run time.
  static Result<int> find_value(const std::string& _str) {
    bool found = false;
    const auto idx = find_value_set_idx(
        _str, &found, std::make_integer_sequence<int, num_fields_>());
    if (!found) {
      return error(
          "LiteralWithDefault does not support string '" + _str +
          "'. The following strings are supported: " + allowed_strings() + ".");
    }
    return idx;
  }

  template <int... _is>
  static int find_value_set_idx(const std::string& _str, bool* _found,
                                std::integer_sequence<int, _is...>) {
    int idx = 0;
    (find_value_set_if_matches<_is>(_str, _found, &idx), ...);
    return idx;
  }

  template <int _i>
  static void find_value_set_if_matches(const std::string& _str, bool* _found,
                                        int* _idx) {
    using FieldType = tuple_element_t<_i, FieldsType>;
    if (!*_found && FieldType::name_.string_view() == _str) {
      *_idx = _i;
      *_found = true;
    }
  }

  /// Finds the value of a string literal at compile time.
  template <internal::StringLiteral _name, int _i = 0>
  static constexpr int find_value_of() {
    if constexpr (_i == num_fields_) {
      return -1;
    } else {
      using FieldType = tuple_element_t<_i, FieldsType>;
      if constexpr (FieldType::name_ == _name) {
        return _i;
      } else {
        return find_value_of<_name, _i + 1>();
      }
    }
  }

  /// Whether the literal contains this string.
  static void has_value(const std::string& _str, bool* _found) {
    find_value_set_idx(_str, _found,
                       std::make_integer_sequence<int, num_fields_>());
  }

  static_assert(sizeof...(fields_) <= std::numeric_limits<ValueType>::max(),
                "Too many fields.");

  static_assert(sizeof...(fields_) <= 1 || !has_duplicates(),
                "Duplicate strings are not allowed in a LiteralWithDefault.");

 private:
  /// The underlying value.
  ValueType value_;
};

}  // namespace rfl

namespace std {
template <rfl::internal::StringLiteral... fields>
struct hash<rfl::LiteralWithDefault<fields...>> {
  size_t operator()(const rfl::LiteralWithDefault<fields...>& _l) const {
    return hash<int>()(static_cast<int>(_l.value()));
  }
};

}  // namespace std

