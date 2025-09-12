#pragma once

#include <cinttypes>

namespace idk::base {

//  FIXME[katayad]: should be std::start_lifetime_as;

template<typename Object>
Object* start_lifetime_as(char* byte) {
  // FIXME[katayad] placement new as for now?
  return reinterpret_cast<Object*>(byte);
}

template<typename Object>
const Object* start_lifetime_as(const char* byte) {
  // FIXME[katayad] placement new as for now?
  return reinterpret_cast<const Object*>(byte);
}

template<typename Object>
Object* start_lifetime_as(uint8_t* byte) {
  // FIXME[katayad] placement new as for now?
  return reinterpret_cast<Object*>(byte);
}

template<typename Object>
const Object* start_lifetime_as(const uint8_t* byte) {
  // FIXME[katayad] placement new as for now?
  return reinterpret_cast<const Object*>(byte);
}

}
