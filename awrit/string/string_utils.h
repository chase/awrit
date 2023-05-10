// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_STRING_STRING_UTILS_H
#define AWRIT_STRING_STRING_UTILS_H

#include <string_view>
#include <vector>

namespace string {
std::vector<std::string_view> split(const std::string_view& str,
                                    char delimiter);
}

#endif  // AWRIT_STRING_STRING_UTILS_H
