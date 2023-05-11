// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <vector>

#include "kitty_keys.h"
#include "third_party/keycodes/keyboard_codes_posix.h"

TEST(KittyKeysTest, LeftArrow) {
  std::string input = "D";
  auto result = tty::keys::KeyEventFromCSI(input);
  ASSERT_TRUE(result);
  EXPECT_EQ(result->windows_key_code, KeyboardCode::VKEY_LEFT);
}
