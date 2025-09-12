#pragma once

#include <cstdint>

namespace idk::net::tcp {

enum class Flags : uint8_t {
  FIN = 0x01,
  SYN = 0x02,
  RST = 0x04,
  PSH = 0x08,
  ACK = 0x10,
  URG = 0x20,
  ECE = 0x40,
  CWR = 0x80,

  SYN_ACK = 0x12,
  FIN_ACK = 0x11,
  RST_ACK = 0x14,
  PSH_ACK = 0x18,
};


inline constexpr Flags operator|(Flags lhs, Flags rhs) noexcept {
  return static_cast<Flags>(
    static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
  );
}


inline constexpr Flags operator&(Flags lhs, Flags rhs) noexcept {
  return static_cast<Flags>(
    static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
  );
}

inline constexpr Flags operator^(Flags lhs, Flags rhs) noexcept {
  return static_cast<Flags>(
    static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs)
  );
}


inline constexpr Flags operator~(Flags flag) noexcept {
  return static_cast<Flags>(~static_cast<uint8_t>(flag));
}


inline constexpr Flags& operator|=(Flags& lhs, Flags rhs) noexcept {
  lhs = lhs | rhs;
  return lhs;
}


inline constexpr Flags& operator&=(Flags& lhs, Flags rhs) noexcept {
  lhs = lhs & rhs;
  return lhs;
}


inline constexpr Flags& operator^=(Flags& lhs, Flags rhs) noexcept {
  lhs = lhs ^ rhs;
  return lhs;
}


inline constexpr bool has_flag(Flags flags, Flags flag) noexcept {
  return (flags & flag) == flag;
}

inline constexpr bool has_flag(uint8_t flags, Flags flag) noexcept {
  return (flags & static_cast<uint8_t>(flag)) == static_cast<uint8_t>(flag);
}

inline constexpr uint8_t to_uint8(Flags flags) noexcept {
  return static_cast<uint8_t>(flags);
}

inline constexpr Flags from_uint8(uint8_t flags) noexcept {
  return static_cast<Flags>(flags);
}

}  // namespace idk::base