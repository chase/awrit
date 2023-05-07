// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_KITTY_KEYS_H
#define AWRIT_KITTY_KEYS_H

#include <type_traits>

namespace KittyKeys {

enum class Modifiers : int {
  shift = 1,
  alt = 2,
  ctrl = 4,
  super = 8,
  hyper = 16,
  meta = 32,
  caps_lock = 64,
  num_lock = 128,
};

enum class Event : int {
  key_down = 1,
  key_repeat = 2,
  key_up = 3,
};

enum class Flags : int {
  disambiguate_escape_codes = 1,
  report_event_types = 2,
  report_alternate_keys = 4,
  report_all_keys_as_escape_codes = 8,
  report_associated_text = 16
};

// allows using | on the above enum classes
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr typename std::underlying_type<T>::type operator|(T lhs, T rhs) {
  return static_cast<typename std::underlying_type<T>::type>(lhs) |
         static_cast<typename std::underlying_type<T>::type>(rhs);
}
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr typename std::underlying_type<T>::type operator|(
    typename std::underlying_type<T>::type lhs, T rhs) {
  return lhs | static_cast<typename std::underlying_type<T>::type>(rhs);
}
// allows using & on the above enum classes
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr typename std::underlying_type<T>::type operator&(T lhs, T rhs) {
  return static_cast<typename std::underlying_type<T>::type>(lhs) &
         static_cast<typename std::underlying_type<T>::type>(rhs);
}
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr typename std::underlying_type<T>::type operator&(
    typename std::underlying_type<T>::type lhs, T rhs) {
  return lhs & static_cast<typename std::underlying_type<T>::type>(rhs);
}

}  // namespace KittyKeys

#endif  // AWRIT_KITTY_KEYS_H
