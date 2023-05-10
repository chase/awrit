// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "shared_memory.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

// Test case for empty buffer
TEST(WriteTest, EmptyBuffer) {
  std::string result = shm::write(nullptr, 0);
  ASSERT_EQ(result, "");
}

// Test case for small buffer
TEST(WriteTest, SmallBuffer) {
  const char test_data[] = "Hello World!";
  std::string result = shm::write(test_data, sizeof(test_data));

  // Ensure write operation is successful
  ASSERT_NE(result, "");

  // Verify the contents of the file
  std::ifstream input_file(result, std::ios::binary);
  ASSERT_TRUE(input_file.is_open());
  std::array<char, sizeof(test_data)> contents;
  input_file.read(contents.data(), contents.size());
  input_file.close();

  // Compare contents
  ASSERT_EQ(std::string(contents.data(), contents.size()),
            std::string(test_data, sizeof(test_data)));

  // Clean up the file
  std::remove(result.c_str());
}

// Test case for larger buffer
TEST(WriteTest, LargeBuffer) {
  const size_t buffer_size = 1024;
  char test_data[buffer_size];
  std::fill_n(test_data, buffer_size, 'A');

  std::string result = shm::write(test_data, buffer_size);

  // Ensure write operation is successful
  ASSERT_NE(result, "");

  // Verify the contents of the file
  std::ifstream input_file(result, std::ios::binary);
  ASSERT_TRUE(input_file.is_open());
  std::array<char, sizeof(test_data)> contents;
  input_file.read(contents.data(), contents.size());
  input_file.close();

  // Compare contents
  ASSERT_EQ(std::string(contents.data(), contents.size()),
            std::string(test_data, sizeof(test_data)));

  // Clean up the file
  std::remove(result.c_str());
}
