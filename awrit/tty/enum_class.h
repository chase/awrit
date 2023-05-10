// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_ENUM_CLASS_H
#define AWRIT_ENUM_CLASS_H

#include <type_traits>

/* enum class utilities that probably shouldn't exist */

// |
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

// &
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

// ^
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr typename std::underlying_type<T>::type operator^(T lhs, T rhs) {
  return static_cast<typename std::underlying_type<T>::type>(lhs) ^
         static_cast<typename std::underlying_type<T>::type>(rhs);
}
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr typename std::underlying_type<T>::type operator^(
    typename std::underlying_type<T>::type lhs, T rhs) {
  return lhs ^ static_cast<typename std::underlying_type<T>::type>(rhs);
}

#endif  // AWRIT_ENUM_CLASS_H
