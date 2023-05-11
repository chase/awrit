// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "string_utils.h"

#include <charconv>

namespace string {

std::vector<std::string_view> split(const std::string_view& str,
                                    char delimiter) {
  if (str.empty()) return {};

  std::vector<std::string_view> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);
  while (end != std::string_view::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }
  tokens.push_back(str.substr(start));

  return tokens;
}

std::optional<int> strtoint(std::string_view str) {
  int result;
  const std::from_chars_result conv =
      std::from_chars(str.begin(), str.end(), result);
  if (conv.ec != std::errc()) return {};
  return result;
}

}  // namespace string
