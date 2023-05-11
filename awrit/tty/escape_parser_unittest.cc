// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "escape_parser.h"

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <vector>

#include "build/_deps/googletest-src/googletest/include/gtest/gtest.h"
#include "tty/escape_codes.h"

TEST(EscapeParserTest, LeftArrow) {
  std::string input = CSI "D";
  char delimiter = ';';
  class TestParser : public tty::EscapeCodeParser {
    bool HandleCSI(const std::string &str) override {
      EXPECT_EQ(str, "D");
      return true;
    };
  };
  TestParser parser;
  parser.Parse(input);
}

TEST(EscapeParserTest, MouseMove) {
  std::string input = CSI "<35;474;141M";
  char delimiter = ';';
  class TestParser : public tty::EscapeCodeParser {
    bool HandleCSI(const std::string &str) override {
      EXPECT_EQ(str, "<35;474;141M");
      return true;
    };
  };
  TestParser parser;
  parser.Parse(input);
}
