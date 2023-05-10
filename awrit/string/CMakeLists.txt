# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

project(string)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(STRING_SRCS
  string_utils.h
  string_utils.cc
  )

source_group(string ${STRING_SRCS})
add_library(string STATIC ${STRING_SRCS})

target_include_directories(string PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_options(string PRIVATE /W4 /WX)
elseif(UNIX)
    target_compile_options(string PRIVATE -Wall -Wextra -Werror -pedantic)
endif()
