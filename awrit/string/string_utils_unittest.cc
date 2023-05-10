#include <gtest/gtest.h>

#include <string_view>
#include <vector>

#include "string_utils.h"

using namespace string;

TEST(SplitTest, EmptyString) {
  std::string_view input = "";
  std::vector<std::string_view> expected_output = {};
  char delimiter = ';';

  auto result = split(input, delimiter);
  ASSERT_EQ(expected_output, result);
}

TEST(SplitTest, SingleCharacterDelimiter) {
  std::string_view input = "a;b;c";
  std::vector<std::string_view> expected_output = {"a", "b", "c"};
  char delimiter = ';';

  auto result = split(input, delimiter);
  ASSERT_EQ(expected_output, result);
}

TEST(SplitTest, NoDelimiterInString) {
  std::string_view input = "abc";
  std::vector<std::string_view> expected_output = {"abc"};
  char delimiter = ';';

  auto result = split(input, delimiter);
  ASSERT_EQ(expected_output, result);
}

TEST(SplitTest, ConsecutiveDelimiters) {
  std::string_view input = "a;;b;c;";
  std::vector<std::string_view> expected_output = {"a", "", "b", "c", ""};
  char delimiter = ';';

  auto result = split(input, delimiter);
  ASSERT_EQ(expected_output, result);
}

TEST(SplitTest, OtherDelimiter) {
  std::string_view input = "a:b:c";
  std::vector<std::string_view> expected_output = {"a", "b", "c"};
  char delimiter = ':';

  auto result = split(input, delimiter);
  ASSERT_EQ(expected_output, result);
}
